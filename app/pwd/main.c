#include "../../include/malloc.c"
#include "../../include/stat.c"
#include "../../include/mem.c"
#include "../../include/dirent.c"
struct dname
{
	struct dname *next;
	char name[256];
} *dname_head;
int get_dir_name(int fd,unsigned long ino,unsigned long dev)
{
	struct dname *node;
	struct DIR db;
	struct dirent *dir;
	struct stat st;
	int err;
	dir_init(fd,&db);
	while(dir=readdir(&db))
	{
		if(err=fstatat(fd,dir->name,&st,AT_SYMLINK_NOFOLLOW))
		{
			exit(err);
		}
		if(st.ino==ino&&st.dev==dev)
		{
			if(strcmp(dir->name,".")&&strcmp(dir->name,".."))
			{
				node=malloc(sizeof(node));
				if(node==NULL)
				{
					exit(2);
				}
				strcpy(node->name,dir->name);
				node->next=dname_head;
				dname_head=node;
				return 1;
			}
			return 0;
		}
	}
}
int main(void)
{
	struct stat st;
	int cfd,pfd,err;
	struct dname *node;
	cfd=AT_FDCWD;
	do
	{
		pfd=openat(cfd,"..",0,0);
		if(pfd<0)
		{
			return pfd;
		}
		if(err=fstatat(cfd,".",&st,AT_SYMLINK_NOFOLLOW))
		{
			return err;
		}
		if(cfd>=0)
		{
			close(cfd);
		}
		cfd=pfd;
	}
	while(get_dir_name(pfd,st.ino,st.dev));
	node=dname_head;
	while(node)
	{
		write(1,"/",1);
		write(1,node->name,strlen(node->name));
		node=node->next;
	}
	write(1,"/\n",2);
	return 0;
}
