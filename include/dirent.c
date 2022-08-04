#ifndef _DIRENT_C_
#define _DIRENT_C_
#include "syscall.c"
#define DIRP_SIZE 1016
struct dirent
{
	unsigned long int ino;
	unsigned long int off;
	unsigned short reclen;
	unsigned char type;
	char name[1];
};
struct DIR
{
	int fd;
	short off;
	short size;
	unsigned char buf[DIRP_SIZE];
};
void dir_init(int fd,struct DIR *dp)
{
	dp->fd=fd;
	dp->off=0;
	dp->size=0;
}
struct dirent *readdir(struct DIR *dp)
{
	struct dirent *ret;
	if(dp->off==dp->size)
	{
		dp->off=0;
		dp->size=getdents64(dp->fd,dp->buf,DIRP_SIZE);
		if(dp->size<=0)
		{
			dp->size=0;
			return NULL;
		}
	}
	ret=(void *)(dp->buf+dp->off);
	dp->off+=ret->reclen;
	return ret;
}

#endif
