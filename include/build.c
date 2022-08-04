#ifndef _BUILD_C_
#define _BUILD_C_
#ifndef _BUILD_INTERNAL_
#error "Never use this file in applications."
#endif
#include "syscall.c"
#include "mem.c"
#include "iformat.c"
#include "ioctl/loop.c"
#include "dirent.c"
#include "stat.c"
long int exec_program(char *name,char **argv)
{
	long int ret,pid;
	int status;
	pid=fork();
	if(!valid(pid))
	{
		return pid;
	}
	if(pid==0)
	{
		execv(name,argv);
		exit(1);
	}
	ret=wait(&status);
	if(!valid(ret))
	{
		return ret;
	}
	if(status)
	{
		return status;
	}
	return 0;
}
void xexec_program(char *name,char **argv)
{
	long int ret;
	if(ret=exec_program(name,argv))
	{
		exit(1);
	}
}
void cc(char *name,char *out)
{
	char *argv[4];
	write(1,"cc ",3);
	write(1,name,strlen(name));
	write(1," ",1);
	write(1,out,strlen(out));
	write(1,"\n",1);
	argv[3]=NULL;
	argv[0]="x";
	argv[1]=name;
	argv[2]="build/tmp/cc.i";
	xexec_program("bin/scpp",argv);
	argv[1]="build/tmp/cc.i";
	argv[2]="build/tmp/cc.bcode";
	xexec_program("bin/scc",argv);
	argv[1]="build/tmp/cc.bcode";
	argv[2]=out;
	xexec_program("bin/bcode",argv);
}
void assemble(char *name,char *out)
{
	char *argv[4];
	write(1,"asm ",4);
	write(1,name,strlen(name));
	write(1," ",1);
	write(1,out,strlen(out));
	write(1,"\n",1);
	argv[3]=NULL;
	argv[0]="x";
	argv[1]=name;
	argv[2]=out;
	xexec_program("bin/asm",argv);
}
void xmkdir(char *path,int mode)
{
	write(1,"mkdir ",6);
	write(1,path,strlen(path));
	write(1,"\n",1);
	if(mkdir(path,mode))
	{
		exit(2);
	}
}
void xappend(char *src,char *dst)
{
	int fdi,fdo;
	static char buf[4096];
	int x;
	write(1,"append ",7);
	write(1,src,strlen(src));
	write(1," ",1);
	write(1,dst,strlen(dst));
	write(1,"\n",1);
	fdi=open(src,0,0);
	if(fdi<0)
	{
		exit(3);
	}
	fdo=open(dst,65,0644);
	if(fdo<0)
	{
		exit(4);
	}
	lseek(fdo,0,2);
	while((x=read(fdi,buf,4096))>0)
	{
		write(fdo,buf,x);
	}
	close(fdi);
	close(fdo);
}
void xsappend(char *str,char *dst)
{
	int fdo;
	int x;
	write(1,"sappend \"",9);
	write(1,str,strlen(str));
	write(1,"\" ",2);
	write(1,dst,strlen(dst));
	write(1,"\n",1);
	fdo=open(dst,65,0644);
	if(fdo<0)
	{
		exit(4);
	}
	lseek(fdo,0,2);
	write(fdo,str,strlen(str));
	close(fdo);
}
void xbappend(void *buf,int size,char *dst)
{
	int fdo;
	int x;
	write(1,"bappend ",8);
	write(1,dst,strlen(dst));
	write(1,"\n",1);
	fdo=open(dst,65,0);
	if(fdo<0)
	{
		exit(4);
	}
	lseek(fdo,0,2);
	write(fdo,buf,size);
	close(fdo);
}
void xiappend(unsigned long int a,char *dst)
{
	char buf[32];
	int x;
	unsigned long int n;
	if(a==0)
	{
		xsappend("0",dst);
		return;
	}
	n=10000000000000000000;
	while(n>a)
	{
		n/=10;
	}
	x=0;
	while(n)
	{
		buf[x]=a/n+'0';
		a%=n;
		n/=10;
		++x;
	}
	buf[x]=0;
	xsappend(buf,dst);
}
#endif
