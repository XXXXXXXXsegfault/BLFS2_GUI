#include "../../include/syscall.c"
#include "../../include/socket.c"
#include "../../include/iformat.c"
#include "../../include/mem.c"

unsigned int gethostbyname(char *host)
{
	char *argv[3];
	int pid,status,n;
	int pipefd[2];
	char buf[32],*p;
	unsigned long int val;
	unsigned int addr;
	argv[0]="gethostbyname";
	argv[1]=host;
	argv[2]=NULL;
	if(pipe(pipefd))
	{
		return 0;
	}
	pid=fork();
	if(pid<0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return 0;
	}
	if(pid==0)
	{
		dup2(pipefd[0],0);
		dup2(pipefd[1],1);
		execv("/bin/gethostbyname",argv);
		exit(1);
	}
	status=0xffffffff;
	waitpid(pid,&status,0);
	if(status)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return 0;
	}
	memset(buf,0,32);
	read(pipefd[0],buf,32);
	close(pipefd[0]);
	close(pipefd[1]);
	p=buf;
	n=0;
	addr=0;
	while(n<4)
	{
		val=0;
		p=sinputi(p,&val)+1;
		addr|=(val&0xff)<<n*8;
		++n;
	}
	return addr;
}
char hostname[70];
unsigned int server_addr;
int sockfds[2];
int fd;
int sock_init(void)
{
	int pid;
	int pfds[4];
	char buf[48];
	char *argv[6];
	alarm(10);
	if(socketpair(AF_UNIX,SOCK_STREAM,0,sockfds))
	{
		return 1;
	}
	pid=fork();
	if(pid<0)
	{
		return 1;
	}
	if(pid==0)
	{
		buf[0]=0;
		sprinti(buf,server_addr&0xff,1);
		strcat(buf,".");
		sprinti(buf,server_addr>>8&0xff,1);
		strcat(buf,".");
		sprinti(buf,server_addr>>16&0xff,1);
		strcat(buf,".");
		sprinti(buf,server_addr>>24&0xff,1);
		strcat(buf,":443");
		close(sockfds[0]);
		dup2(sockfds[1],0);
		close(sockfds[1]);
		dup2(0,1);
		argv[0]="openssl";
		argv[1]="s_client";
		argv[2]="-quiet";
		argv[3]="-connect";
		argv[4]=buf;
		argv[5]=NULL;
		execv("/opt/openssl",argv);
		exit(1);
	}
	close(sockfds[1]);
	return 0;
}
void send_req(char *str)
{
	write(sockfds[0],str,strlen(str));
}
unsigned char buf[65536];
int main(int argc,char **argv)
{
	long int l,total_l;
	int x;
	char *req;
	int n;
	
	if(argc<3)
	{
		return 1;
	}
	fd=open(argv[2],578,0644);
	if(fd<0)
	{
		return 2;
	}
	l=strlen(argv[1]);
	if(l>64)
	{
		l=64;
	}
	memcpy(hostname,argv[1],l);
	x=0;
	while(hostname[x])
	{
		if(hostname[x]=='/')
		{
			hostname[x]=0;
			break;
		}
		++x;
	}
	server_addr=gethostbyname(hostname);
	if(server_addr==0)
	{
		// Unknown host
		return 3;
	}
	if(sock_init())
	{
		// Error trying to open socket
		return 4;
	}
	send_req("GET ");
	if(argv[1][x]==0)
	{
		send_req("/");
	}
	else
	{
		send_req(argv[1]+x);
	}
	send_req(" HTTP/1.1\r\n");
	send_req("Host: ");
	send_req(hostname);
	send_req("\r\n");
	send_req("Referer: ");
	send_req(argv[1]);
	send_req("\r\n");
	send_req("Connection: close\r\n");
	send_req("Accept: text/html\r\n");
	send_req("User-Agent: HTTPGET (BLFS2 Linux)\r\n\r\n");
	total_l=0;
	while(1)
	{
		alarm(30);
		l=read(sockfds[0],buf,65536);
		if(l<=0)
		{
			break;
		}
		write(fd,buf,l);
		total_l+=l;
		if(total_l>0x200000)
		{
			return 5;
		}
	}
	close(fd);
	return 0;
}
