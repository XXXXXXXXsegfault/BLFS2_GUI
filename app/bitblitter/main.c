#include "../../include/malloc.c"
#include "../../include/mem.c"
#include "../../include/iformat.c"
#include "../../include/sock_read.c"

int cpu_supports_sse;
int cpu_supports_avx;

#include "memcpy.c"
#include "libdrm.c"
#include "display.c"
#define SOCKPATH "/tmp/bitblitter.socket"

int shmid;
unsigned int *shm;

int sockfd;
int server_init(void)
{
	struct sockaddr_un addr;
	unlink(SOCKPATH);
	sockfd=socket(AF_UNIX,SOCK_STREAM,0);
	if(sockfd<0)
	{
		return 1;
	}
	memset(&addr,0,sizeof(addr));
	addr.family=AF_UNIX;
	strcpy(addr.sun_path,SOCKPATH);
	if(bind(sockfd,&addr,sizeof(addr)))
	{
		return 1;
	}
	if(listen(sockfd,2))
	{
		return 1;
	}
	if(chmod(SOCKPATH,0600))
	{
		return 1;
	}
	shmid=shmget('W',(long)mode->hdisplay*mode->vdisplay*4+4096,01600);
	shm=shmat(shmid,NULL,0);
	if(!valid(shm))
	{
		return 1;
	}
	return 0;
}
#include "server.c"
int main(void)
{
	int cfd;
	int pid;
	if(getuid()!=0)
	{
		return 1;
	}
	sched_setscheduler(gettid(),2,"\x63\x00\x00\x00");
	nice(-20);
	if(!test_sse())
	{
		cpu_supports_sse=1;
	}
	if(!test_avx())
	{
		cpu_supports_avx=1;
	}
	if(display_init())
	{
		return 1;
	}
	if(server_init())
	{
		return 1;
	}
	pid=fork();
	if(pid>0)
	{
		sleep(0,200000);
		return 0;
	}
	if(pid<0)
	{
		return 1;
	}
	while(1)
	{
		sleep(0,100000);
		if((cfd=accept(sockfd,NULL,NULL))>=0)
		{
			handle_req(cfd);
			close(cfd);
		}
	}
}
