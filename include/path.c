#ifndef _PATH_C_
#define _PATH_C_
#include "syscall.c"
#include "mem.c"
#include "stat.c"
int fissubdir(int dirfd,int fd)
{
	int fd1,fd2;
	int ret;
	struct stat st,dirst,dirst_old;
	if(ret=fstat(dirfd,&st))
	{
		return ret;
	}
	fd1=dup(fd);
	if(fd1<0)
	{
		return fd1;
	}
	fd2=-1;
	while(1)
	{
			
		if(ret=fstat(fd1,&dirst))
		{
			close(fd1);
			return ret;
		}
		if(dirst.ino==st.ino&&dirst.dev==st.dev)
		{
			close(fd1);
			return 1;
		}
		if(fd2!=-1&&dirst.ino==dirst_old.ino&&dirst.dev==dirst_old.dev)
		{
			close(fd1);
			return 0;
		}
		memcpy(&dirst_old,&dirst,sizeof(dirst));
		fd2=openat(fd1,"..",0,0);
		close(fd1);
		if(fd2<0)
		{
			return fd2;
		}
		fd1=fd2;
	}
}
int dirname_open(char *path,char **path_ret)
{
	int fd1,fd2;
	char buf[270];
	int x,c;
	char *path1;
	if(!strcmp(path,"/"))
	{
		if(path_ret)
		{
			*path_ret=path;
		}
		return open("/",0,0);
	}
	fd1=AT_FDCWD;
	if(*path=='/')
	{
		fd1=open("/",0,0);
		if(fd1<0)
		{
			return fd1;
		}
		++path;
	}
	path1=path;
	x=0;
	while(c=*path)
	{
		if(c=='/')
		{
			buf[x]=0;
			do
			{
				++path;
			}
			while(*path=='/');
			if(*path)
			{
				fd2=openat(fd1,buf,0,0);
				close(fd1);
				if(fd2<0)
				{
					return fd2;
				}
				fd1=fd2;
				path1=path;
			}
			x=0;
		}
		else
		{
			if(x>=256)
			{
				return -ENAMETOOLONG;
			}
			buf[x]=c;
			++path;
			++x;
		}
	}
	if(path_ret)
	{
		*path_ret=path1;
	}
	if(fd1==AT_FDCWD)
	{
		fd1=open(".",0,0);
	}
	return fd1;
}
int openl(char *path,int flags,int mode)
{
	int dir,fd;
	char *bname;
	dir=dirname_open(path,&bname);
	if(dir<0)
	{
		return dir;
	}
	fd=openat(dir,bname,flags,mode);
	close(dir);
	return fd;
}
int statl(char *path,struct stat *st)
{
	int dir,ret;
	char *bname;
	dir=dirname_open(path,&bname);
	if(dir<0)
	{
		return dir;
	}
	ret=fstatat(dir,bname,st,0);
	close(dir);
	return ret;
}
int lstatl(char *path,struct stat *st)
{
	int dir,ret;
	char *bname;
	dir=dirname_open(path,&bname);
	if(dir<0)
	{
		return dir;
	}
	ret=fstatat(dir,bname,st,AT_SYMLINK_NOFOLLOW);
	close(dir);
	return ret;
}
int mkdirl(char *path,int mode)
{
	int dir,ret;
	char *bname;
	dir=dirname_open(path,&bname);
	if(dir<0)
	{
		return dir;
	}
	ret=mkdirat(dir,bname,mode);
	close(dir);
	return ret;
}
int issubdir(char *dirpath,char *path)
{
	int dirfd,fd,ret;
	dirfd=openl(dirpath,0,0);
	if(dirfd<0)
	{
		return dirfd;
	}
	fd=dirname_open(path,NULL);
	if(fd<0)
	{
		close(dirfd);
		return fd;
	}
	ret=fissubdir(dirfd,fd);
	close(dirfd);
	close(fd);
	return ret;
}
int dirname_openat(int dirfd,char *path,char **path_ret)
{
	int fd1,fd2;
	char buf[270];
	int x,c;
	char *path1;
	if(!strcmp(path,"/"))
	{
		if(path_ret)
		{
			*path_ret=path;
		}
		return open("/",0,0);
	}
	if(dirfd==AT_FDCWD)
	{
		fd1=open(".",0,0);
	}
	else
	{
		fd1=dup(dirfd);
	}
	if(*path=='/')
	{
		close(fd1);
		fd1=open("/",0,0);
		if(fd1<0)
		{
			return fd1;
		}
		++path;
	}
	else if(fd1<0)
	{
		return fd1;
	}
	path1=path;
	x=0;
	while(c=*path)
	{
		if(c=='/')
		{
			buf[x]=0;
			do
			{
				++path;
			}
			while(*path=='/');
			if(*path)
			{
				fd2=openat(fd1,buf,0,0);
				close(fd1);
				if(fd2<0)
				{
					return fd2;
				}
				fd1=fd2;
				path1=path;
			}
			x=0;
		}
		else
		{
			if(x>=256)
			{
				return -ENAMETOOLONG;
			}
			buf[x]=c;
			++path;
			++x;
		}
	}
	if(path_ret)
	{
		*path_ret=path1;
	}
	if(fd1==AT_FDCWD)
	{
		fd1=open(".",0,0);
	}
	return fd1;
}
int openatl(int dirfd,char *path,int flags,int mode)
{
	int dir,fd;
	char *bname;
	dir=dirname_openat(dirfd,path,&bname);
	if(dir<0)
	{
		return dir;
	}
	fd=openat(dir,bname,flags,mode);
	close(dir);
	return fd;
}
int mkdiratl(int dirfd,char *path,int mode)
{
	int dir,ret;
	char *bname;
	dir=dirname_openat(dirfd,path,&bname);
	if(dir<0)
	{
		return dir;
	}
	ret=mkdirat(dir,bname,mode);
	close(dir);
	return ret;
}
int fchmodatl(int dirfd,char *path,int mode)
{
	int dir,ret;
	char *bname;
	dir=dirname_openat(dirfd,path,&bname);
	if(dir<0)
	{
		return dir;
	}
	ret=fchmodat(dir,bname,mode);
	close(dir);
	return ret;
}
int fstatatl(int dirfd,char *path,struct stat *st,int flags)
{
	int dir,ret;
	char *bname;
	dir=dirname_openat(dirfd,path,&bname);
	if(dir<0)
	{
		return dir;
	}
	ret=fstatat(dir,bname,st,flags);
	close(dir);
	return ret;
}
int symlinkatl(char *target,int dirfd,char *path)
{
	int dir,ret;
	char *bname;
	dir=dirname_openat(dirfd,path,&bname);
	if(dir<0)
	{
		return dir;
	}
	ret=symlinkat(target,dir,bname);
	close(dir);
	return ret;
}
int readlinkatl(int dirfd,char *path,char *buf,int size)
{
	int dir,ret;
	char *bname;
	dir=dirname_openat(dirfd,path,&bname);
	if(dir<0)
	{
		return dir;
	}
	ret=readlinkat(dir,bname,buf,size);
	close(dir);
	return ret;
}
int unlinkatl(int dirfd,char *path,int flags)
{
	int dir,ret;
	char *bname;
	dir=dirname_openat(dirfd,path,&bname);
	if(dir<0)
	{
		return dir;
	}
	ret=unlinkat(dir,bname,flags);
	close(dir);
	return ret;
}
int renameatl(int dirfd,char *path,int newdirfd,char *newpath)
{
	int dir,newdir,ret;
	char *bname,*new_bname;
	dir=dirname_openat(dirfd,path,&bname);
	if(dir<0)
	{
		return dir;
	}
	newdir=dirname_openat(newdirfd,newpath,&new_bname);
	if(newdir<0)
	{
		close(dir);
		return newdir;
	}
	ret=renameat(dir,bname,newdir,new_bname);
	close(dir);
	close(newdir);
	return ret;
}

#endif
