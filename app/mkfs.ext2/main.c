#include "../../include/syscall.c"
#include "../../include/mem.c"
#include "../../include/ext2.c"
int fd;
unsigned char buf[131072];
unsigned int buf_x;
struct ext2_superblock sb;
struct ext2_bgdt *gdt;
unsigned long groups,gt_blocks;
unsigned long size,blocks;
unsigned int last_group_size;
unsigned char block_bitmap[4096];
unsigned char inode_bitmap[4096];

int init_fs_info(void)
{
	unsigned long x;
	unsigned int nb[3];
	int has_super;
	sb.block_size=2;
	sb.frag_size=2;
	sb.blocks_per_group=32768;
	sb.frags_per_group=32768;
	sb.inodes_per_group=8192;
	sb.max_mounts=0xffff;
	sb.magic=0xef53;
	sb.state=1;
	sb.errors=1;
	sb.rev=1;
	sb.first_ino=11;
	sb.inode_size=256;
	sb.feature_incompat=2;
	sb.feature_ro_compat=3;
	getrandom(sb.uuid,16,1);
	size=lseek(fd,0,2);
	if(!valid(size))
	{
		return 1;
	}
	blocks=size>>12;
	if(blocks==0)
	{
		return 1;
	}
	last_group_size=blocks&32767;
	if(last_group_size==0)
	{
		last_group_size=32768;
	}
	groups=blocks+32767>>15;
	if(groups==0)
	{
		return 1;
	}
	gt_blocks=groups+127>>7;
	x=1;
	nb[0]=1;
	nb[1]=5;
	nb[2]=7;
	has_super=1;
	while(x<groups)
	{
		if(x==nb[0])
		{
			has_super=1;
			nb[0]*=3;
		}
		else if(x==nb[1])
		{
			has_super=1;
			nb[1]*=5;
		}
		else if(x==nb[2])
		{
			has_super=1;
			nb[2]*=7;
		}
		else
		{
			has_super=0;
		}
		++x;
	}
	if(has_super)
	{
		if(gt_blocks+(1+1+1+512+1)>last_group_size)
		{
			blocks-=last_group_size;
			last_group_size=32768;
			--groups;
		}
	}
	else
	{
		if(1+1+512+1>last_group_size)
		{
			blocks-=last_group_size;
			last_group_size=32768;
			--groups;
		}
	}
	if(groups==0||blocks>0xffffffff)
	{
		return 1;
	}
	gt_blocks=groups+127>>7;
	gdt=mmap(0,gt_blocks<<12,3,0x22,-1,0);
	if(!valid(gdt))
	{
		return 1;
	}

	sb.blocks=blocks;
	sb.inodes=groups*8192;
	sb.free_inodes=sb.inodes-10;

	gdt[0].free_inodes=8192-10;
	gdt[0].block_bitmap=1+gt_blocks;
	gdt[0].inode_bitmap=1+gt_blocks+1;
	gdt[0].inode_table=1+gt_blocks+1+1;
	gdt[0].used_dirs=1;

	if(groups==1)
	{
		gdt[0].free_blocks=last_group_size-(1+gt_blocks+1+1+512+1);
	}
	else
	{
		gdt[0].free_blocks=32768-(1+gt_blocks+1+1+512+1);
	}
	sb.free_blocks=gdt[0].free_blocks;

	x=1;
	nb[0]=1;
	nb[1]=5;
	nb[2]=7;
	while(x<groups)
	{
		if(x==nb[0])
		{
			has_super=1;
			nb[0]*=3;
		}
		else if(x==nb[1])
		{
			has_super=1;
			nb[1]*=5;
		}
		else if(x==nb[2])
		{
			has_super=1;
			nb[2]*=7;
		}
		else
		{
			has_super=0;
		}
		gdt[x].free_inodes=8192;
		if(has_super)
		{
			gdt[x].block_bitmap=(x<<15)+1+gt_blocks;
			if(x==groups-1)
			{
				gdt[x].free_blocks=last_group_size-(1+gt_blocks+1+1+512);
			}
			else
			{
				gdt[x].free_blocks=32768-(1+gt_blocks+1+1+512);
			}
		}
		else
		{
			gdt[x].block_bitmap=x<<15;
			if(x==groups-1)
			{
				gdt[x].free_blocks=last_group_size-(1+1+512);
			}
			else
			{
				gdt[x].free_blocks=32768-(1+1+512);
			}
		}
		gdt[x].inode_bitmap=gdt[x].block_bitmap+1;
		gdt[x].inode_table=gdt[x].inode_bitmap+1;
		sb.free_blocks+=gdt[x].free_blocks;
		++x;
	}
	sb.r_blocks=sb.free_blocks/128;
	memset(inode_bitmap+1024,0xff,3072);
	return 0;
}
void bitmap_set(unsigned char *bitmap,int n)
{
	int x;
	x=0;
	while(n>=8)
	{
		bitmap[x]=0xff;
		n-=8;
		++x;
	}
	bitmap[x]|=(1<<n)-1;
}
void bitmap_set2(unsigned char *bitmap,int n)
{
	int x;
	x=4096;
	while(n>=8)
	{
		--x;
		bitmap[x]=0xff;
		n-=8;
	}
	--x;
	bitmap[x]|=0x100-(1<<8-n);
}
void buf_write(void *ptr,unsigned long int size)
{
	while(size)
	{
		if(size>=131072-buf_x)
		{
			memcpy(buf+buf_x,ptr,131072-buf_x);
			size-=131072-buf_x;
			ptr=(char *)ptr+131072-buf_x;
			buf_x=0;
			write(fd,buf,131072);
		}
		else
		{
			memcpy(buf+buf_x,ptr,size);
			buf_x+=size;
			ptr=(char *)ptr+size;
			size=0;
		}
	}
}
void buf_write_zero(unsigned long int size)
{
	while(size)
	{
		if(size>=131072-buf_x)
		{
			memset(buf+buf_x,0,131072-buf_x);
			size-=131072-buf_x;
			buf_x=0;
			write(fd,buf,131072);
		}
		else
		{
			memset(buf+buf_x,0,size);
			buf_x+=size;
			size=0;
		}
	}
}
void buf_flush(void)
{
	if(buf_x)
	{
		write(fd,buf,buf_x);
		buf_x=0;
	}
}

void write_first_group(void)
{
	struct ext2_inode inode;
	unsigned char buf[12];
	struct ext2_directory *dir;
	lseek(fd,1024,0);
	buf_write(&sb,1024);
	buf_write_zero(2048);
	buf_write(gdt,gt_blocks*4096);
	bitmap_set(block_bitmap,1+gt_blocks+1+1+512+1);
	if(groups==1)
	{
		bitmap_set2(block_bitmap,32768-last_group_size);
	}
	bitmap_set(inode_bitmap,10);
	buf_write(block_bitmap,4096);
	buf_write(inode_bitmap,4096);
	buf_write_zero(256);
	memset(&inode,0,128);
	inode.mode=040755;
	inode.links=2;
	inode.size=4096;
	inode.blocks=8;
	inode.block[0]=1+gt_blocks+1+1+512;
	buf_write(&inode,128);
	buf_write_zero(128+256*8190);
	memset(buf,0,12);
	dir=(void *)buf;
	dir->inode=2;
	dir->rec_len=12;
	dir->name_len=1;
	dir->file_type=2;
	dir->file_name[0]='.';
	buf_write(dir,12);
	dir->rec_len=4096-12;
	dir->name_len=2;
	dir->file_name[1]='.';
	buf_write(dir,12);
	buf_write_zero(4096-12-12);
	buf_flush();
}
void write_group(unsigned long n,int has_super)
{
	lseek(fd,n<<27,0);
	if(has_super)
	{
		buf_write(&sb,1024);
		buf_write_zero(3072);
		buf_write(gdt,gt_blocks*4096);
	}
	memset(block_bitmap,0,4096);
	if(n==groups-1)
	{
		bitmap_set(block_bitmap,last_group_size-gdt[n].free_blocks);
		bitmap_set2(block_bitmap,32768-last_group_size);
	}
	else
	{
		bitmap_set(block_bitmap,32768-gdt[n].free_blocks);
	}
	buf_write(block_bitmap,4096);
	buf_write(inode_bitmap,4096);
	buf_write_zero(256*8192);
	buf_flush();
}
int main(int argc,char **argv)
{
	unsigned long x;
	unsigned int nb[3];
	if(argc<2)
	{
		return 1;
	}
	fd=open(argv[1],2,0);
	if(fd<0)
	{
		return 1;
	}
	if(init_fs_info())
	{
		return 1;
	}
	write_first_group();
	memset(inode_bitmap,0,1024);
	x=1;
	nb[0]=1;
	nb[1]=5;
	nb[2]=7;
	while(x<groups)
	{
		if(x==nb[0])
		{
			nb[0]*=3;
			write_group(x,1);
		}
		else if(x==nb[1])
		{
			nb[1]*=5;
			write_group(x,1);
		}
		else if(x==nb[2])
		{
			nb[2]*=7;
			write_group(x,1);
		}
		else
		{
			write_group(x,0);
		}
		++x;
	}
	return 0;
}
