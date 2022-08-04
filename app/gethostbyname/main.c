#include "../../include/syscall.c"
#include "../../include/mem.c"
#include "../../include/socket.c"
#include "../../include/iformat.c"
struct dns_msg
{
	unsigned short id;
	unsigned short flags;
	unsigned short questions;
	unsigned short answers;
	unsigned short answers2[2];
	unsigned char buf[65496];
} dns_msg;
unsigned int dns_addr;
int dnsfd;
int dns_init(void)
{
	struct sockaddr_in addr;
	dnsfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(dnsfd<0)
	{
		return 1;
	}
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr=dns_addr;
	addr.sin_port=htonw(53);
	if(connect(dnsfd,&addr,sizeof(addr)))
	{
		close(dnsfd);
		return 1;
	}
	return 0;
}
int main(int argc,char **argv)
{
	int fd,n;
	unsigned long int val;
	char buf[32],*p;
	unsigned short id;
	int x,x1,s;
	if(argc<2)
	{
		return 1;
	}
	if(strlen(argv[1])>64)
	{
		return 1;
	}
	fd=open("/run/dns_addr",0,0);
	if(fd<0)
	{
		return 1;
	}
	alarm(3);
	memset(buf,0,32);
	read(fd,buf,32);
	close(fd);
	p=buf;
	n=0;
	while(n<4)
	{
		val=0;
		p=sinputi(p,&val);
		dns_addr|=(val&0xff)<<n*8;
		++p;
		++n;
	}
	if(dns_init())
	{
		return 1;
	}
	getrandom(&id,2,1);
	dns_msg.id=id;
	dns_msg.flags=1; // RD
	dns_msg.questions=htonw(1);
	strcpy(dns_msg.buf+1,argv[1]);
	x=1;
	x1=0;
	while(dns_msg.buf[x])
	{
		if(dns_msg.buf[x]=='.')
		{
			x1=x;
			dns_msg.buf[x]=0;
		}
		else
		{
			++dns_msg.buf[x1];
		}
		++x;
	}
	++x;
	dns_msg.buf[x+1]=1;
	dns_msg.buf[x+3]=1;
	write(dnsfd,&dns_msg,x+4+12);
	memset(&dns_msg,0,sizeof(dns_msg));
	if((n=read(dnsfd,&dns_msg,65508))<0)
	{
		return 1;
	}
	if(dns_msg.id!=id||(dns_msg.flags&0xf00))
	{
		return 1;
	}
	x1=htonw(dns_msg.questions);
	x=0;
	n-=12;
	while(x1&&x<n)
	{
		while(dns_msg.buf[x])
		{
			++x;
		}
		x+=5;
		--x1;
	}
	x1=htonw(dns_msg.answers);
	while(x1&&x<n)
	{
		if((dns_msg.buf[x]&0xc0)==0xc0)
		{
			x+=2;
		}
		else
		{
			while(dns_msg.buf[x])
			{
				++x;
			}
			++x;
		}
		s=0;
		if(dns_msg.buf[x+1]==1&&dns_msg.buf[x+3]==1)
		{
			s=1;
		}
		x+=8;
		val=dns_msg.buf[x+1];
		val=val<<8|dns_msg.buf[x];
		val=htonw(val);
		x+=2;
		if(s&&val==4)
		{
			buf[0]=0;
			sprinti(buf,(unsigned int)dns_msg.buf[x],1);
			strcat(buf,".");
			sprinti(buf,(unsigned int)dns_msg.buf[x+1],1);
			strcat(buf,".");
			sprinti(buf,(unsigned int)dns_msg.buf[x+2],1);
			strcat(buf,".");
			sprinti(buf,(unsigned int)dns_msg.buf[x+3],1);
			strcat(buf,"\n");
			write(1,buf,strlen(buf));
			return 0;
		}
		x+=val;
		--x1;
	}
	return 1;
}
