#ifndef _SOCK_READ_C_
#define _SOCK_READ_C_
#include "syscall.c"
#include "socket.c"
#include "poll.c"
int sock_read(int fd,void *buf,int size)
{
	struct pollfd pfd;
	int n,x;
	n=0;
	while(n<size)
	{
		pfd.fd=fd;
		pfd.events=POLLIN;
		pfd.revents=0;
		if(poll(&pfd,1,-1)==1)
		{
			if(pfd.revents&POLLHUP)
			{
				break;
			}
			else if(pfd.revents&POLLIN)
			{
				x=read(fd,buf,size-n);
				if(x<0)
				{
					break;
				}
				n+=x;
				buf=(char *)buf+x;
			}
		}
	}
	return n;
}
#endif
