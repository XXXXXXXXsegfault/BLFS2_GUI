#define _BUILD_INTERNAL_
#include "boot/build.c"
#include "app/build.c"
#include "install/build.c"
#include "include/malloc.c"
#include "include/ioctl/termios.c"

void copy_file(char *src,char *dst)
{
	char *argv[4];
	argv[0]="copy";
	argv[1]=src;
	argv[2]=dst;
	argv[3]=0;
	xexec_program("build/root/bin/copy",argv);
}
void mkrootid(void)
{
	int fd;
	char rootid[16];
	fd=open("build/tmp/rootid",578,0644);
	if(fd<0)
	{
		exit(1);
	}
	getrandom(rootid,16,1);
	write(fd,rootid,16);
	close(fd);
}
int is_ko(char *name)
{
	int l;
	if(!strcmp(name,"bzImage"))
	{
		return 1;
	}
	l=strlen(name);
	if(l>3&&!memcmp(name+l-3,".ko",3))
	{
		return 1;
	}
	return 0;
}
void copy_ko(int dstfd,char *dst,int srcfd,char *src)
{
	struct stat st;
	int fdi,fdo,n;
	struct DIR db;
	struct dirent *dir;
	static char buf[131072];
	if(fstatat(srcfd,src,&st,AT_SYMLINK_NOFOLLOW))
	{
		exit(20);
	}
	fdi=openat(srcfd,src,0,0);
	if(fdi<0)
	{
		exit(21);
	}
	if((st.mode&0170000)==STAT_DIR)
	{
		mkdirat(dstfd,dst,0755);
		fdo=openat(dstfd,dst,0,0);
		if(fdo<0)
		{
			exit(22);
		}
		dir_init(fdi,&db);
		while(dir=readdir(&db))
		{
			if(strcmp(dir->name,".")&&strcmp(dir->name,".."))
			{
				copy_ko(fdo,dir->name,fdi,dir->name);
			}
		}
		close(fdo);
		unlinkat(dstfd,dst,AT_REMOVEDIR);
	}
	else if((st.mode&0170000)==STAT_REG)
	{
		if(is_ko(src))
		{
			fdo=openat(dstfd,dst,577,0644);
			if(fdo<0)
			{
				exit(23);
			}
			while((n=read(fdi,buf,131072))>0)
			{
				write(fdo,buf,n);
			}
			close(fdo);
		}
	}
	close(fdi);
}
void _scan_ko(int modlist,char *path,int load)
{
	int fd;
	int l;
	struct DIR db;
	struct dirent *dir;
	struct stat st;
	char *new_path;
	if(lstat(path,&st))
	{
		exit(31);
	}
	if((st.mode&0170000)==STAT_DIR)
	{
		fd=open(path,0,0);
		if(fd<0)
		{
			exit(32);
		}
		dir_init(fd,&db);
		l=strlen(path);
		new_path=malloc(l+264);
		if(new_path==NULL)
		{
			exit(33);
		}
		memcpy(new_path,path,l);
		while(dir=readdir(&db))
		{
			if(strcmp(dir->name,".")&&strcmp(dir->name,".."))
			{
				new_path[l]='/';
				strcpy(new_path+l+1,dir->name);
				if(!strcmp(dir->name,"hid")||!strcmp(dir->name,"drm")||!strcmp(dir->name,"ethernet")||!strcmp(dir->name,"usb")||!strcmp(dir->name,"crypto"))
				{
					_scan_ko(modlist,new_path,1);
				}
				else if(!strcmp(dir->name,"phy"))
				{
					_scan_ko(modlist,new_path,2);
				}
				else
				{
					_scan_ko(modlist,new_path,load);
				}
			}
		}
		free(new_path);
		close(fd);
	}
	else
	{
		l=strlen(path);
		if(!memcmp(path+l-3,".ko",3))
		{
			if(!load)
			{
				if(write(modlist,"#",1)!=1)
				{
					exit(34);
				}
			}
			else if(load==2)
			{
				if(write(modlist,"!",1)!=1)
				{
					exit(34);
				}
			}
			if(write(modlist,path+10,l-10)!=l-10)
			{
				exit(34);
			}
			if(write(modlist,"\n",1)!=1)
			{
				exit(34);
			}
		}
	}
}
void scan_ko(void)
{
	int modlist;
	modlist=open("build/root/etc/modules",577,0644);
	if(modlist<0)
	{
		exit(30);
	}
	_scan_ko(modlist,"build/root/lib/linux",0);
	close(modlist);
}
void init_fd(void)
{
	struct termios term;
	int fd;
	fd=open("build.log",578,0644);
	if(fd<0)
	{
		exit(1);
	}
	if(ioctl(1,TCGETS,&term))
	{
		dup2(fd,1);
	}
	if(ioctl(2,TCGETS,&term))
	{
		dup2(fd,2);
	}
	close(fd);
}
int main(void)
{
	char *msg;
	char *argv[5];
	init_fd();
	msg="Start Building\n";
	write(1,msg,strlen(msg));
	xmkdir("build",0755);
	xmkdir("build/tmp",0755);
	xmkdir("build/boot",0755);
	xmkdir("build/root",0755);
	xmkdir("build/root/bin",0755);
	xmkdir("build/root/lib",0755);
	xmkdir("build/root/opt",0755);
	xmkdir("build/mnt",0755);
	xmkdir("build/initramfs",0755);
	build_boot("build/boot/bootx64.efi");
	build_apps();
	mkrootid();
	copy_file("build/tmp/rootid","build/initramfs/rootid");
	copy_file("bin/scpp","build/root/bin/scpp");
	copy_file("bin/scc","build/root/bin/scc");
	copy_file("bin/bcode","build/root/bin/bcode");
	copy_file("bin/asm","build/root/bin/asm");
	argv[0]="cpio";
	argv[1]="-p";
	argv[2]="build/initramfs";
	argv[3]="build/tmp/initramfs";
	argv[4]=0;
	xexec_program("build/root/bin/cpio",argv);
	xmkdir("build/root/src",0755);
	copy_file("app","build/root/src/app");
	copy_file("boot","build/root/src/boot");
	copy_file("build.c","build/root/src/build.c");
	copy_file("include","build/root/src/include");
	copy_file("include","build/root/include");
	copy_file("install","build/root/src/install");
	copy_file("syscfg","build/root");
	copy_file("syscfg","build/root/src/syscfg");
	copy_ko(AT_FDCWD,"build/root/lib/linux",AT_FDCWD,"src/linux");
	scan_ko();
	copy_file("src/openssl/apps/openssl","build/root/opt/openssl");
	xmkdir("build/root/src/scc",0755);
	xmkdir("build/root/src/src",0755);
	xmkdir("build/root/src/src/openssl",0755);
	xmkdir("build/root/src/src/openssl/apps",0755);
	copy_file("src/openssl/apps/openssl","build/root/src/src/openssl/apps/openssl");
	copy_file("scc/asm","build/root/src/scc/asm");
	copy_file("scc/bcode","build/root/src/scc/bcode");
	copy_file("scc/include","build/root/src/scc/include");
	copy_file("scc/scc","build/root/src/scc/scc");
	copy_file("scc/scpp","build/root/src/scc/scpp");
	build_installer("build/install.run");
	write(1,"OK\n",3);

	return 0;
}
