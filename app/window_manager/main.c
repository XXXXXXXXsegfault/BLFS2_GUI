#define _WM_SERVER_

#include "../../include/malloc.c"
#include "../../include/mem.c"
#include "../../include/lwp.c"
#include "../../include/sock_read.c"
#include "../../include/bitblitter.c"
#include "../../include/signal.c"
#include "../../include/rect.c"
#include "../../include/iformat.c"
#include "../../include/window_manager.c"

int bbfd,sockfd;
unsigned int *bg_map;
unsigned short int bg_size[2];
int shmid;
unsigned int *shm;

int init_socket(char *path,int mode)
{
	struct sockaddr_un addr;
	int fd;
	unlink(path);
	fd=socket(AF_UNIX,SOCK_STREAM,0);
	if(fd<0)
	{
		return -1;
	}
	memset(&addr,0,sizeof(addr));
	addr.family=AF_UNIX;
	strcpy(addr.sun_path,path);
	if(bind(fd,&addr,sizeof(addr)))
	{
		return -1;
	}
	if(listen(fd,32))
	{
		return -1;
	}
	chmod(path,mode);
	return fd;
}
int server_init(void)
{
	struct sockaddr_un addr;
	bbfd=socket(AF_UNIX,SOCK_STREAM,0);
	if(bbfd<0)
	{
		return 1;
	}
	memset(&addr,0,sizeof(addr));
	addr.family=AF_UNIX;
	strcpy(addr.sun_path,"/tmp/bitblitter.socket");
	if(connect(bbfd,&addr,sizeof(addr)))
	{
		return 1;
	}
	if((sockfd=init_socket("/tmp/windowm.socket",0666))<0)
	{
		return 1;
	}
	mkdir("/run/wmshm",0711);
	return 0;
}
void load_background(void)
{
	int fd;
	long int size;
	fd=open("/etc/background.bin",0,0);
	if(fd<0)
	{
		return;
	}
	if(read(fd,bg_size,4)!=4)
	{
		close(fd);
		return;
	}
	size=lseek(fd,0,2);
	if(size<0||size!=(unsigned long int)bg_size[0]*bg_size[1]*4+4)
	{
		close(fd);
		return;
	}
	size-=4;
	lseek(fd,4,0);
	bg_map=malloc(size);
	if(bg_map==NULL)
	{
		close(fd);
		return;
	}
	if(read(fd,bg_map,size)!=size)
	{
		free(bg_map);
		bg_map=NULL;
		close(fd);
		return;
	}
	close(fd);
}

void SH_success(int sig)
{
	sleep(0,200000);
	exit(0);
}
void SH_error(int sig)
{
	sleep(0,200000);
	exit(1);
}
#include "display.c"
#include "input.c"
struct server_args
{
	int cfd;
	unsigned int size;
	struct window *win[MAX_WINDOWS];
	char shm[96];
	void *databuf;
};
#include "functions.c"
int create_wmshm(struct server_args *sargs)
{
	char buf[96];
	unsigned char key[32];
	int x,fd;
	unsigned char c,c1,c2;
	memset(buf,0,sizeof(buf));
	if(getrandom(key,32,1)!=32)
	{
		return 1;
	}
	strcpy(buf,"/run/wmshm/");
	x=0;
	while(x<32)
	{
		c=key[x];
		c1=c>>4;
		c2=c&0xf;
		if(c1<10)
		{
			c1+='0';
		}
		else
		{
			c1+='A'-10;
		}
		if(c2<10)
		{
			c2+='0';
		}
		else
		{
			c2+='A'-10;
		}
		buf[11+x*2]=c1;
		buf[11+x*2+1]=c2;
		++x;
	}
	fd=open(buf,578|0200,0644);
	if(fd<0)
	{
		return 1;
	}
	lseek(fd,screen_w*screen_h*4-1,0);
	write(fd,"\x00",1);
	sargs->databuf=mmap(0,screen_w*screen_h*4,3,1,fd,0);
	close(fd);
	chmod(buf,0666);
	if(!valid(sargs->databuf))
	{
		unlink(buf);
		return 1;
	}
	write(sargs->cfd,buf,96);
	memcpy(sargs->shm,buf,96);
	return 0;
}
int T_service(void *ptr)
{
	struct server_args *sargs;
	struct wmmsg req;
	int x;
	unsigned int size;
	char buf[256];
	sargs=ptr;
	while(1)
	{
		if(sock_read(sargs->cfd,&req,sizeof(req))!=sizeof(req))
		{
			x=0;
			mutex_lock(&display_lock);
			while(x<MAX_WINDOWS)
			{
				if(sargs->win[x])
				{
					delete_window(sargs->win[x]);
				}
				++x;
			}
			mutex_unlock(&display_lock);
			close(sargs->cfd);
			munmap(sargs->databuf,screen_w*screen_h*4);
			unlink(sargs->shm);
			free(sargs);
			paint=1;
			return 0;
		}
		if(req.data_size>screen_w*screen_h*4)
		{
			req.cmd=0xffffffff;
		}
		else
		{
			sargs->size=req.data_size;
			if(wmcall_handler(sargs,&req))
			{
				req.cmd=0xffffffff;
			}
		}
		write(sargs->cfd,&req,sizeof(req));
	}
}
int T_server(void *args)
{
	int cfd;
	struct server_args *sargs;
	while(1)
	{
		if((cfd=accept(sockfd,NULL,NULL))>=0)
		{
				sargs=malloc(sizeof(*sargs));
				if(sargs==NULL)
				{
					close(cfd);
				}
				else
				{
					memset(sargs,0,sizeof(*sargs));
					sargs->cfd=cfd;
					if(create_wmshm(sargs))
					{
						free(sargs);
						close(cfd);
					}
					else if(!valid(create_lwp(16384,T_service,sargs)))
					{
						munmap(sargs->databuf,screen_w*screen_h*4);
						unlink(sargs->shm);
						free(sargs);
						close(cfd);
					}
				}
		}
		sleep(0,100000);
	}
}
int main(void)
{
	int pid;
	if(getuid()!=0)
	{
		return 1;
	}
	sched_setscheduler(gettid(),2,"\x63\x00\x00\x00");
	nice(-20);
	if(server_init())
	{
		return 1;
	}
	load_background();
	display_init();
	signal(SIGPIPE,SIG_IGN);
	pid=fork();
	if(pid>0)
	{
		signal(SIGINT,SH_success);
		signal(SIGQUIT,SH_error);
		pause();
	}
	if(pid<0)
	{
		return 1;
	}
	if(!valid(create_lwp(8192,T_server,NULL)))
	{
		kill(getppid(),SIGQUIT);
		return 1;
	}
	if(!valid(create_lwp(4096,T_input,NULL)))
	{
		kill(getppid(),SIGQUIT);
		return 1;
	}
	if(!valid(create_lwp(4096,T_mice,NULL)))
	{
		kill(getppid(),SIGQUIT);
		return 1;
	}
	kill(getppid(),SIGINT);
	while(1)
	{
		mutex_lock(&display_lock);
		if(lock_set32(&paint,0))
		{
			display_all_nolock();
			display_buf_flush();
		}
		mutex_unlock(&display_lock);
		sleep(0,80000/screen_refresh);
	}
}
