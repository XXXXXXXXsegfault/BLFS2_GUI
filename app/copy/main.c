#include "../../include/path.c"
#include "../../include/dirent.c"
void copy_link(int olddirfd,char *oldname,int newdirfd,char *newname)
{
	static char buf[4100];
	int n;
	n=readlinkatl(olddirfd,oldname,buf,4099);
	if(n<0)
	{
		exit(1);
	}
	buf[n]=0;
	if(symlinkatl(buf,newdirfd,newname))
	{
		exit(1);
	}
}
void copy_file(int olddirfd,char *oldname,int newdirfd,char *newname,int mode)
{
	static char buf[0x10000];
	int n;
	int fdi,fdo;
	fdi=openatl(olddirfd,oldname,0,0);
	if(fdi<0)
	{
		exit(1);
	}
	fdo=openatl(newdirfd,newname,706,0644);
	if(fdo<0)
	{
		exit(1);
	}
	while((n=read(fdi,buf,0x10000))>0)
	{
		if(write(fdo,buf,n)!=n)
		{
			exit(1);
		}
	}
	close(fdi);
	close(fdo);
	fchmodatl(newdirfd,newname,mode);
}
void copy_dir(int olddirfd,char *oldname,int newdirfd,char *newname,int mode)
{
	int fdi,fdo;
	struct DIR db;
	struct dirent *dir;
	struct stat st;
	static int n;
	mkdiratl(newdirfd,newname,0755);
	fdi=openatl(olddirfd,oldname,0,0);
	if(fdi<0)
	{
		exit(1);
	}
	fdo=openatl(newdirfd,newname,0,0);
	if(fdo<0)
	{
		exit(1);
	}
	if(fissubdir(fdi,fdo))
	{
		exit(1);
	}
	dir_init(fdi,&db);
	while(dir=readdir(&db))
	{
		if(strcmp(dir->name,".")&&strcmp(dir->name,".."))
		{
			if(!fstatat(fdi,dir->name,&st,AT_SYMLINK_NOFOLLOW))
			{
				if((st.mode&0170000)==STAT_LNK)
				{
					copy_link(fdi,dir->name,fdo,dir->name);
				}
				else if((st.mode&0170000)==STAT_REG)
				{
					copy_file(fdi,dir->name,fdo,dir->name,st.mode&07777);
				}
				else if((st.mode&0170000)==STAT_DIR)
				{
					copy_dir(fdi,dir->name,fdo,dir->name,st.mode&07777);
				}
			}
		}
	}
	close(fdi);
	close(fdo);
	fchmodatl(newdirfd,newname,mode);
}
int main(int argc,char **argv)
{
	struct stat st;
	int status;
	if(argc<3)
	{
		return 1;
	}
	if(lstatl(argv[1],&st))
	{
		return 1;
	}
	if((st.mode&0170000)==STAT_LNK)
	{
		copy_link(AT_FDCWD,argv[1],AT_FDCWD,argv[2]);
	}
	else if((st.mode&0170000)==STAT_REG)
	{
		copy_file(AT_FDCWD,argv[1],AT_FDCWD,argv[2],st.mode&07777);
	}
	else if((st.mode&0170000)==STAT_DIR)
	{
		copy_dir(AT_FDCWD,argv[1],AT_FDCWD,argv[2],st.mode&07777);
	}
	return 0;
}
