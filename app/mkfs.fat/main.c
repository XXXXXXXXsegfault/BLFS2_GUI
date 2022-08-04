#include "../../include/syscall.c"
#include "../../include/mem.c"
#include "../../include/fat32.c"
int fd;
struct fat32_bpb bsec;
struct fat32_fsinfo fsinfo;
unsigned long size,sectors,clusters;
int init_fs_info(void)
{
	unsigned long int fat_size;
	size=lseek(fd,0,2);
	if(!valid(size))
	{
		return 1;
	}
	sectors=size>>9;
	if(sectors<=512)
	{
		return 1;
	}
	memcpy(bsec.jump,"\xeb\xfe\x90",3);
	memcpy(bsec.oem_id,"BLinuxfS",8);
	bsec.sector_size=512;
	bsec.reserved_sectors=512;
	bsec.fats=2;
	bsec.media_type=0xf8;
	bsec.part_off=0;
	bsec.root_cluster=2;
	bsec.fsinfo=1;
	bsec.backup_bootsec=6;
	bsec.drvnum=0x80;
	bsec.boot_sign=0x29;
	getrandom(&bsec.id,4,1);
	memcpy(bsec.volume_name,"NO NAME    ",11);
	memcpy(bsec.fstype,"FAT32   ",8);
	bsec.bootflag=0xaa55;
	
	bsec.sectors_per_cluster=1;
	fsinfo.signature=0x41615252;
	fsinfo.signature2=0x61417272;
	fsinfo.signature3=0xaa550000;
	fsinfo.start_cluster=2;

	while(1)
	{
		if(bsec.sectors_per_cluster==64)
		{
			return 1;
		}
		clusters=(sectors-512)*512/(512*bsec.sectors_per_cluster+4);
		if(clusters)
		{
			fat_size=clusters+2>>7;
			fat_size+=bsec.sectors_per_cluster-1;
			fat_size-=fat_size%bsec.sectors_per_cluster;
			while(clusters*bsec.sectors_per_cluster+fat_size*2+512>sectors)
			{
				--clusters;
			}
			if(clusters&&clusters<0xffffff0)
			{
				sectors=clusters*bsec.sectors_per_cluster+fat_size*2+512;
				break;
			}
		}
		bsec.sectors_per_cluster<<=1;
	}
	bsec.fat_size_large=fat_size;
	if(sectors>65535)
	{
		if(sectors>0xffffffff)
		{
			return 1;
		}
		bsec.sectors_large=sectors;
	}
	else
	{
		bsec.sectors=sectors;
	}
	fsinfo.free_clusters=clusters-1;
	return 0;
}
unsigned char buf[131072];
unsigned int buf_x;
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
void write_fat(void)
{
	unsigned int value;
	value=0xffffff8;
	buf_write(&value,4);
	value=0xfffffff;
	buf_write(&value,4);
	value=0xffffff8;
	buf_write(&value,4);
	buf_write_zero(bsec.fat_size_large*512-12);
}

int main(int argc,char **argv)
{
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
	lseek(fd,0,0);
	buf_write(&bsec,512);
	buf_write(&fsinfo,512);
	buf_flush();
	lseek(fd,6*512,0);
	buf_write(&bsec,512);
	buf_flush();
	lseek(fd,512*512,0);
	write_fat();
	write_fat();
	buf_write_zero((unsigned int)bsec.sectors_per_cluster*512);
	buf_flush();
	return 0;
}
