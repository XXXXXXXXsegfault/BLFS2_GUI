struct ext2_superblock ext2_sb;
unsigned int ext2_cache[1024*6];
unsigned int ext2_cache_block[6];
unsigned char _ext2_cache2[262144+16384];
#define ext2_cache2 ((unsigned char *)((long)_ext2_cache2+16384&~16383))
unsigned long ext2_cache2_base,ext2_cache2_size;
unsigned char ext2_block[4096];
struct ext2_inode ext2_inode_current;
unsigned long ext2_groups;
struct ext2_bgdt *ext2_bgdt;
unsigned long int ext2_read_blocks(unsigned long off,unsigned long size,void *buf)
{
	if(off<ext2_cache2_base||off*bio->media->blocksize+size>=ext2_cache2_base*bio->media->blocksize+ext2_cache2_size)
	{
		ext2_cache2_size=read_blocks(bio,off,262144,ext2_cache2);
		ext2_cache2_base=off;
		if(ext2_cache2_size<size)
		{
			memcpy(buf,ext2_cache2,ext2_cache2_size);
			return ext2_cache2_size;
		}
	}
	memcpy(buf,ext2_cache2+(off-ext2_cache2_base)*bio->media->blocksize,size);
	return size;
}
void *ext2_read_block(unsigned long nblock)
{
	unsigned long ssize;
	unsigned long off,off2;
	if(nblock==0)
	{
		memset(ext2_block,0,4096);
		return ext2_block;
	}
	if(nblock>=ext2_sb.blocks)
	{
		return 0;
	}
	ssize=bio->media->blocksize;
	off=nblock<<ext2_sb.block_size+10;
	off2=off%ssize;
	off/=ssize;
	if(ext2_read_blocks(off,4096,ext2_block)<off2+(1<<ext2_sb.block_size+10))
	{
		return 0;
	}
	return ext2_block+off2;
}
int ext2_load_inode(unsigned int ino)
{
	unsigned int group,index,block,bi;
	unsigned int block_size;
	unsigned char *ptr;
	struct ext2_bgdt *gdt;
	block_size=1<<ext2_sb.block_size+10;
	group=(ino-1)/ext2_sb.inodes_per_group;
	if(group>=ext2_groups)
	{
		return 1;
	}
	index=(ino-1)%ext2_sb.inodes_per_group;
	block=index*ext2_sb.inode_size/block_size;
	bi=index*ext2_sb.inode_size%block_size;
	ptr=ext2_read_block(ext2_bgdt[group].inode_table+block);
	if(ptr==0)
	{
		return 1;
	}
	memcpy(&ext2_inode_current,ptr+bi,128);
	return 0;
}
struct ext2_image
{
	unsigned long size;
	char *data;
};
void *ext2_cache_load(int n,unsigned int block)
{
	void *ptr;
	if(ext2_cache_block[n]!=block)
	{
		ptr=ext2_read_block(block);
		if(ptr==0)
		{
			return 0;
		}
		memcpy(ext2_cache+n*1024,ptr,1<<ext2_sb.block_size+10);
		ext2_cache_block[n]=block;
	}
	return ext2_cache+n*1024;
}
void *ext2_file_read_block(unsigned int nblock)
{
	unsigned int n,n1,n2,n3;
	unsigned int *cache;
	if(nblock<12)
	{
		return ext2_read_block(ext2_inode_current.block[nblock]);
	}
	nblock-=12;
	n=1<<ext2_sb.block_size+8;
	if(nblock<n)
	{
		cache=ext2_cache_load(0,ext2_inode_current.block[12]);
		if(cache==0)
		{
			return 0;
		}
		return ext2_read_block(cache[nblock]);
	}
	nblock-=n;
	n<<=ext2_sb.block_size+8;
	if(nblock<n)
	{
		n1=nblock>>ext2_sb.block_size+8;
		n2=nblock&(1<<ext2_sb.block_size+8)-1;
		cache=ext2_cache_load(1,ext2_inode_current.block[13]);
		if(cache==0)
		{
			return 0;
		}
		cache=ext2_cache_load(2,cache[n1]);
		if(cache==0)
		{
			return 0;
		}
		return ext2_read_block(cache[n2]);
	}
	nblock-=n;
	n<<=ext2_sb.block_size+8;
	if(nblock<n)
	{
		n1=nblock>>2*(ext2_sb.block_size+8);
		n2=nblock>>ext2_sb.block_size+8&(1<<ext2_sb.block_size+8)-1;
		n3=nblock&(1<<ext2_sb.block_size+8)-1;
		cache=ext2_cache_load(3,ext2_inode_current.block[14]);
		if(cache==0)
		{
			return 0;
		}
		cache=ext2_cache_load(4,cache[n1]);
		if(cache==0)
		{
			return 0;
		}
		cache=ext2_cache_load(5,cache[n2]);
		if(cache==0)
		{
			return 0;
		}
		return ext2_read_block(cache[n3]);
	}
	return 0;
}
int ext2_image_load(struct ext2_image *image)
{
	unsigned long pages,block,x;
	void *data;
	image->size=ext2_inode_current.size;
	if((ext2_inode_current.mode&0170000)==0100000)
	{
		image->size|=(unsigned long)ext2_inode_current.dir_acl<<32;
	}
	else if((ext2_inode_current.mode&0170000)!=040000)
	{
		return 1;
	}
	pages=image->size+4095>>12;
	if(!pages)
	{
		image->data=0;
		return 0;
	}
	image->data=palloc(pages);
	if(image->data==0)
	{
		return 1;
	}
	x=0;
	block=0;
	while(x<image->size)
	{
		data=ext2_file_read_block(block);
		if(data==0)
		{
			prelease(image->data,pages);
			return 1;
		}
		memcpy(image->data+x,data,1<<ext2_sb.block_size+10);
		x+=1<<ext2_sb.block_size+10;
		++block;
	}
	return 0;
}
void ext2_image_release(struct ext2_image *image)
{
	unsigned long pages;
	pages=image->size+4095>>12;
	prelease(image->data,pages);
}
int ext2_load_dir(char *name)
{
	struct ext2_image image;
	struct ext2_directory *dir;
	unsigned int x,namelen;
	if((ext2_inode_current.mode&0170000)!=040000)
	{
		return 1;
	}
	if(ext2_image_load(&image))
	{
		return 1;
	}
	namelen=strlen(name);
	if(namelen==0)
	{
		ext2_image_release(&image);
		return 0;
	}
	x=0;
	while(x<image.size)
	{
		dir=(void *)(image.data+x);
		if(dir->rec_len==0)
		{
			ext2_image_release(&image);
			return 1;
		}
		if(x+dir->rec_len>image.size)
		{
			ext2_image_release(&image);
			return 1;
		}
		if(namelen==dir->name_len&&!memcmp(dir->file_name,name,namelen)&&dir->inode!=0)
		{
			if(ext2_load_inode(dir->inode))
			{
				ext2_image_release(&image);
				return 1;
			}
			ext2_image_release(&image);
			return 0;
		}
		x+=dir->rec_len;
	}
	ext2_image_release(&image);
	return 1;
}
int ext2_load_path(char *path)
{
	char name[300];
	int x,x1;
	x=0;
	x1=0;
	if(ext2_load_inode(2))
	{
		return 1;
	}
	if((ext2_inode_current.mode&0170000)!=040000)
	{
		return 1;
	}
	while(path[x])
	{
		if(path[x]=='/')
		{
			name[x1]=0;
			if(x1)
			{
				if(ext2_load_dir(name))
				{
					return 1;
				}
			}
			x1=0;
		}
		else
		{
			if(x1>=256)
			{
				return 1;
			}
			name[x1]=path[x];
			++x1;
		}
		++x;
	}
	name[x1]=0;
	if(x1)
	{
		if(ext2_load_dir(name))
		{
			return 1;
		}
	}
	if((ext2_inode_current.mode&0170000)!=0100000)
	{
		return 1;
	}
	return 0;
}
int ext2_init(void)
{
	unsigned long int start_block,blocks,x;
	void *ptr;
	if(read_blocks(bio,0,4096,ext2_block)!=4096)
	{
		return 1;
	}
	memcpy(&ext2_sb,ext2_block+1024,1024);
	if(ext2_sb.block_size>2||ext2_sb.magic!=0xef53||ext2_sb.feature_incompat!=2)
	{
		return 1;
	}
	ext2_groups=ext2_sb.blocks;
	ext2_groups+=ext2_sb.blocks_per_group-1;
	ext2_groups/=ext2_sb.blocks_per_group;
	if(ext2_sb.block_size)
	{
		start_block=1;
	}
	else
	{
		start_block=2;
	}
	blocks=(ext2_groups-1>>ext2_sb.blocks+5)+1;
	ext2_bgdt=palloc((blocks<<ext2_sb.block_size+10)+4095>>12);
	if(ext2_bgdt==0)
	{
		return 1;
	}
	x=0;
	while(x<blocks)
	{
		ptr=ext2_read_block(start_block);
		if(ptr==0)
		{
			return 1;
		}
		memcpy(ext2_bgdt+(x<<ext2_sb.block_size+5),ptr,1<<ext2_sb.block_size+10);
		++x;
		++start_block;
	}
	return 0;
}
