#define _BUILD_INTERNAL_
#include "../include/build.c"
#include "../include/gpt.c"
#include "../include/ioctl/loop.c"
#include "../include/ioctl/termios.c"
#include "../include/lwp.c"
#define EFI_SIZE (47*2048)
#define BOOT_SIZE (48*2048)
#define EFI_START 2048
#define BOOT_START (48*2048)
#define ROOT_START (96*2048)
char loop_name[32];
char *devname;
int loopnum;
int devfd,loopfd;
long int devsize;
long int sectors;
long int root_size;
struct loop_info64 loopinfo;
int loop_init(void)
{
	int ctlfd;
	ctlfd=open("/dev/loop-control",2,0);
	if(ctlfd<0)
	{
		return 1;
	}
	if(!valid(loopnum=ioctl(ctlfd,LOOP_CTL_GET_FREE,0)))
	{
		close(ctlfd);
		return 1;
	}
	strcpy(loop_name,"/dev/loop");
	sprinti(loop_name,loopnum,1);
	loopfd=open(loop_name,2,0);
	if(loopfd<0)
	{
		close(ctlfd);
		return 1;
	}
	if(ioctl(loopfd,LOOP_SET_FD,devfd))
	{
		close(ctlfd);
		return 1;
	}
	if(ioctl(loopfd,LOOP_GET_STATUS64,&loopinfo))
	{
		close(ctlfd);
		return 1;
	}
	close(ctlfd);
	return 0;
}
void loop_fini(void)
{
	static char buf[262144];
	int n;
	ioctl(loopfd,LOOP_CLR_FD,0);
	close(loopfd);
}
void write_pmbr(void)
{
	struct mbr_entry mbr[4];
	lseek(loopfd,0,0);
	write(loopfd,"\xeb\xfe",2);
	lseek(loopfd,0x1be,0);
	memset(mbr,0,64);
	mbr[0].type=0xee;
	mbr[0].lba_start=1;
	if(sectors>0xffffffff)
	{
		mbr[0].lba_size=0xffffffff;
	}
	else
	{
		mbr[0].lba_size=sectors-1;
	}
	write(loopfd,mbr,64);
	write(loopfd,"\x55\xaa",2);
}
void write_gpt(void)
{
	static struct gpt_entry entries[128];
	static struct gpt_header header;
	static char zero[512];

	memcpy(entries[0].type,"\x28\x73\x2a\xc1\x1f\xf8\xd2\x11\xba\x4b\x00\xa0\xc9\x3e\xc9\x3b",16);
	getrandom(entries[0].guid,16,1);
	entries[0].start=EFI_START;
	entries[0].end=entries[0].start+EFI_SIZE-1;
	memcpy(entries[0].name,"E\0F\0I\0 \0S\0y\0s\0t\0e\0m\0 \0P\0a\0r\0t\0i\0t\0i\0o\0n\0",40);

	memcpy(entries[1].type,"\xe3\xbc\x68\x4f\xcd\xe8\xb1\x4d\x96\xe7\xfb\xca\xf9\x84\xb7\x09",16);
	getrandom(entries[1].guid,16,1);
	entries[1].start=BOOT_START;
	entries[1].end=entries[1].start+BOOT_SIZE-1;
	memcpy(entries[1].name,"B\0O\0O\0T\0",8);

	memcpy(entries[2].type,"\xe3\xbc\x68\x4f\xcd\xe8\xb1\x4d\x96\xe7\xfb\xca\xf9\x84\xb7\x09",16);
	getrandom(entries[2].guid,16,1);
	entries[2].start=ROOT_START;
	entries[2].end=entries[2].start+root_size-1;
	memcpy(entries[2].name,"R\0O\0O\0T\0",8);

	memcpy(header.signature,"EFI PART",8);
	header.revision=0x10000;
	header.header_size=0x5c;
	header.lba=1;
	header.lba_alter=sectors-1;
	header.first_usable=34;
	header.last_usable=sectors-34;
	getrandom(header.guid,16,1);
	header.entries_start=2;
	header.entries_count=128;
	header.entry_size=128;
	header.entries_crc32=gpt_crc32(entries,sizeof(entries));
	header.header_crc32=gpt_crc32(&header,0x5c);

	lseek(loopfd,512,0);
	write(loopfd,&header,0x5c);
	write(loopfd,zero,512-0x5c);
	write(loopfd,entries,sizeof(entries));


	lseek(loopfd,sectors-33<<9,0);
	write(loopfd,entries,sizeof(entries));
	
	header.lba=sectors-1;
	header.lba_alter=1;
	header.header_crc32=0;
	header.header_crc32=gpt_crc32(&header,0x5c);
	write(loopfd,&header,0x5c);
	write(loopfd,zero,512-0x5c);
}
void format_loop(char *program)
{
	char *argv[3];
	argv[0]=program;
	argv[1]=loop_name;
	argv[2]=NULL;
	exec_program(program,argv);
}

void copy_file(char *src,char *dst)
{
	char *argv[4];
	argv[0]="copy";
	argv[1]=src;
	argv[2]=dst;
	argv[3]=0;
	exec_program("build/root/bin/copy",argv);
}
char bootid[16];
void read_id(char *name)
{
	int fd;
	fd=open(name,0,0);
	if(fd<0)
	{
		return;
	}
	read(fd,bootid,16);
	close(fd);
}
int main(int argc,char **argv)
{
	int status;
	unsigned long size;
	if(argc<2)
	{
		write(1,"Usage: ./build/install.run DEVICE [RootSize(MB)]\n",49);
		return 1;
	}
	if(getuid()!=0)
	{
		return 1;
	}
	size=0;
	if(argc>=3)
	{
		sinputi(argv[2],&size);
	}
	devname=argv[1];
	devfd=open(argv[1],2,0);
	if(devfd<0)
	{
		return 1;
	}
	devsize=lseek(devfd,0,2);
	if(devsize<0)
	{
		return 1;
	}
	if(loop_init())
	{
		return 1;
	}
	sectors=devsize>>9;
	root_size=sectors-36-ROOT_START&0xfffffffffffff800;
	if(size*2048<root_size)
	{
		root_size=size*2048;
	}
	write_pmbr();
	write_gpt();
	
	//EFI partition
	loopinfo.offset=(EFI_START<<9);
	loopinfo.sizelimit=EFI_SIZE<<9;
	while((status=ioctl(loopfd,LOOP_SET_STATUS64,&loopinfo))==-11);
	if(status<0)
	{
		return 1;
	}

	format_loop("build/root/bin/mkfs.fat");
	if(mount(loop_name,"build/mnt","vfat",0,0))
	{
		return 1;
	}
	mkdir("build/mnt/efi",0755);
	mkdir("build/mnt/efi/boot",0755);
	copy_file("build/boot/bootx64.efi","build/mnt/efi/boot/bootx64.efi");
	while(umount("build/mnt"));
	
	//Boot partition
	loopinfo.offset=(BOOT_START<<9);
	loopinfo.sizelimit=BOOT_SIZE<<9;
	while((status=ioctl(loopfd,LOOP_SET_STATUS64,&loopinfo))==-11);
	if(status<0)
	{
		return 1;
	}

	read_id("build/tmp/bootid");
	lseek(loopfd,0,0);
	write(loopfd,bootid,16);

	format_loop("build/root/bin/mkfs.ext2");
	if(mount(loop_name,"build/mnt","ext2",0,0))
	{
		return 1;
	}
	copy_file("src/linux/arch/x86/boot/bzImage","build/mnt/vmlinuz");
	copy_file("build/tmp/initramfs","build/mnt/initramfs");
	while(umount("build/mnt"));
	
	//Root partition
	loopinfo.offset=(ROOT_START<<9);
	loopinfo.sizelimit=root_size<<9;
	while((status=ioctl(loopfd,LOOP_SET_STATUS64,&loopinfo))==-11);
	if(status<0)
	{
		return 1;
	}

	read_id("build/tmp/rootid");
	lseek(loopfd,0,0);
	write(loopfd,bootid,16);

	format_loop("build/root/bin/mkfs.ext2");
	if(mount(loop_name,"build/mnt","ext2",0,0))
	{
		return 1;
	}
	copy_file("build/root","build/mnt");
	chmod("build/mnt/bin/su",04755);
	copy_file("src/firmware/lib","build/mnt/lib");
	mkdir("build/mnt/home",0700);
	chown("build/mnt/home",4000,4000);
	while(umount("build/mnt"));
	
	loop_fini();
	unlink("build/install.img");
	struct termios term;
	if(ioctl(1,TCGETS,&term))
	{
		mknod("install_success.msg",0100644,0);
	}
	else
	{
		write(1,"Success\n",8);
	}
	return 0;
}
