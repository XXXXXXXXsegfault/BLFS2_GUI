#include "../../include/syscall.c"
#include "../../include/mem.c"
#include "../../include/dirent.c"
#include "../../include/stat.c"
#include "../../include/signal.c"
void hang(void)
{
	write(1,"System Halted\n",14);
	while(1)
	{
		sleep(60,0);
	}
}
void smkdir(char *name)
{
	if(mkdir(name,0755))
	{
		write(1,"mkdir FAILED: ",14);
	}
	else
	{
		write(1,"mkdir ",6);
	}
	write(1,name,strlen(name));
	write(1,"\n",1);
}
void xmount(char *src,char *mp,char *type)
{
	int failed;
	failed=0;
	if(mount(src,mp,type,0,0))
	{
		write(1,"mount FAILED: ",14);
		failed=1;
	}
	else
	{
		write(1,"mount ",6);
	}
	write(1,src,strlen(src));
	write(1," on ",4);
	write(1,mp,strlen(mp));
	write(1," type ",6);
	write(1,type,strlen(type));
	write(1,"\n",1);
	if(failed)
	{
		hang();
	}
}
char rootid[16];
int mount_root(char *path)
{
	int dirfd,fd;
	struct DIR db;
	struct dirent *dir;
	struct stat st;
	char new_path[512];
	char buf[16];
	if(lstat(path,&st))
	{
		return 0;
	}
	if((st.mode&0170000)==STAT_BLK)
	{
		fd=open(path,0,0);
		if(fd<0)
		{
			return 0;
		}
		if(read(fd,buf,16)!=16)
		{
			close(fd);
			return 0;
		}
		close(fd);
		if(memcmp(rootid,buf,16))
		{
			return 0;
		}
		xmount(path,"/root","ext2");
		return 1;
	}
	else if((st.mode&0170000)==STAT_DIR)
	{
		dirfd=open(path,0,0);
		if(dirfd<0)
		{
			return 0;
		}
		dir_init(dirfd,&db);
		while(dir=readdir(&db))
		{
			if(strcmp(dir->name,".")&&strcmp(dir->name,".."))
			{
				strcpy(new_path,path);
				strcat(new_path,"/");
				strcat(new_path,dir->name);
				if(mount_root(new_path))
				{
					close(dirfd);
					return 1;
				}
			}
		}
		close(dirfd);
		return 0;
	}
	return 0;
}
int initfd;
int init_script_getc(void)
{
	static unsigned char buf[4096];
	static int x,size,ret;
	if(x==size)
	{
		x=0;
		size=read(initfd,buf,4096);
		if(size<=0)
		{
			size=0;
			return -1;
		}
	}
	ret=buf[x];
	++x;
	return ret;
}
void remove_tmp(void)
{
	char *argv[3];
	argv[2]=NULL;
	argv[0]="remove";
	argv[1]="/tmp";
	if(fork()==0)
	{
		execv("/bin/remove",argv);
		exit(1);
	}
	wait(NULL);
}
void exec_cmd(char *cmd)
{
	char *argv[3];
	argv[0]="exec";
	argv[1]=cmd;
	argv[2]=NULL;
	execv("/bin/exec",argv);
}
void run_init_script(void)
{
	static char path[4097];
	int x,c;
	int pid;
	initfd=open("/etc/init",02000000,0);
	if(initfd<0)
	{
		write(1,"Cannot open /etc/init\n",27);
		return;
	}
	c=-1;
	do
	{
		x=0;
		while(x<4096)
		{
			c=init_script_getc();
			if(c=='\n'||c==-1)
			{
				path[x]=0;
				break;
			}
			else
			{
				path[x]=c;
			}
			++x;
		}
		if(x)
		{
			pid=fork();
			if(pid==0)
			{
				exec_cmd(path);
				exit(1);
			}
			else if(pid>0)
			{
				wait(NULL);
			}
		}
	}
	while(c!=-1);
	close(initfd);
}
void shutdown(int if_reboot)
{
	write(1,"\033[2J",4);
	sleep(0,300000);
	kill(-1,SIGTERM);
	sleep(3,0);
	kill(-1,SIGKILL);
	sleep(3,0);
	umount("/root/dev");
	umount("/root/proc");
	umount("/root/sys");
	umount("/root/run");
	umount("/root");
	sync();
	while(1)
	{
		if(if_reboot)
		{
			reboot(0xfee1dead,0x20112000,0x01234567,0);
		}
		else
		{
			reboot(0xfee1dead,0x20112000,0x4321fedc,0);
		}
	}
}
void SH_reboot(int sig)
{
	shutdown(1);
}
void SH_shutdown(int sig)
{
	shutdown(0);
}
int main(void)
{
	int fd;
	if(getpid()!=1)
	{
		return 1;
	}
	signal(SIGINT,SH_reboot);
	signal(SIGQUIT,SH_shutdown);
	reboot(0xfee1dead,0x20112000,0,0);
	fd=open("/rootid",0,0);
	if(fd<0)
	{
		write(1,"Cannot open /rootid\n",20);
		hang();
	}
	read(fd,rootid,16);
	close(fd);
	smkdir("/dev");
	smkdir("/root");
	xmount("devtmpfs","/dev","devtmpfs");
	while(!mount_root("/dev"));
	symlink("/root/lib","/lib");
	if(fork()==0)
	{
		chdir("/root");
		chroot(".");
		write(1,"chroot /root\n",13);
		smkdir("/dev");
		smkdir("/proc");
		smkdir("/sys");
		smkdir("/run");
		remove_tmp();
		smkdir("/tmp");
		chmod("/tmp",01777);
		write(1,"chmod /tmp 01777\n",17);
		xmount("devtmpfs","/dev","devtmpfs");
		xmount("proc","/proc","proc");
		xmount("sysfs","/sys","sysfs");
		xmount("tmpfs","/run","tmpfs");
		run_init_script();
		exit(0);
	}
	while(1)
	{
		sleep(0,100000);
		wait(NULL);
	}
}
