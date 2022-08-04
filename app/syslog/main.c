#include "../../include/syscall.c"
char buf[131072];
int main(int argc,char **argv)
{
	int fdi,fdo;
	int n;
	if(argc<2)
	{
		return 1;
	}
	fdi=open("/dev/kmsg",04000,0);
	if(fdi<0)
	{
		return 1;
	}
	fdo=open(argv[1],578,0644);
	if(fdo<0)
	{
		return 1;
	}
	while((n=read(fdi,buf,131072))>0)
	{
		write(fdo,buf,n);
	}
	return 0;
}
