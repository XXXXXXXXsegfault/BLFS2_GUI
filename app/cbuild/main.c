#include "../../include/syscall.c"
#include "../../include/mem.c"
#include "../../include/iformat.c"
void exec_program(char *path,char *arg2,char *arg3,char *arg4)
{
	int pid;
	unsigned int status;
	char buf[96];
	pid=fork();
	if(pid<0)
	{
		write(2,"Cannot create process\n",22);
		exit(1);
	}
	if(pid==0)
	{
		char *argv[4];
		argv[0]=path;
		argv[1]=arg2;
		argv[2]=arg3;
		argv[3]=arg4;
		argv[4]=NULL;
		execv(path,argv);
		exit(1);
	}
	write(1,"run ",4);
	write(1,path,strlen(path));
	write(1,"\n",1);
	status=0;
	waitpid(pid,&status,0);
	if(status)
	{
		if(status&0x7f)
		{
			strcpy(buf,"Program terminated with signal ");
			sprinti(buf,status&0x7f,1);
			strcat(buf,"\n");
			write(2,buf,strlen(buf));
		}
		else
		{
			strcpy(buf,"Program exited with code ");
			sprinti(buf,status>>8&0xff,1);
			strcat(buf,"\n");
			write(2,buf,strlen(buf));
		}
		exit(1);
	}
}
int main(int argc,char **argv)
{
	int fd,execfd;
	if(argc<3)
	{
		return 1;
	}
	fd=open("cbuild.log",578,0644);
	if(fd<0)
	{
		return 1;
	}
	execfd=open("/proc/self/exe",0,0);
	if(execfd<0)
	{
		return 1;
	}
	flock(execfd,2);
	dup2(fd,1);
	dup2(fd,2);
	exec_program("/bin/scpp",argv[1],"cc.i",NULL);
	exec_program("/bin/scc","cc.i","cc.bcode",NULL);
	exec_program("/bin/bcode","cc.bcode","cc.asm",NULL);
	exec_program("/bin/asm","cc.asm",argv[2],"cc.map");
	write(1,"Build Complete\n",15);
	return 0;
}
