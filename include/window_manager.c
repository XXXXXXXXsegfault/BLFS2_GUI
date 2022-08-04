#ifndef _WINDOW_MANAGER_C_
#define _WINDOW_MANAGER_C_
struct wmev
{
	int type;
	int code;
};
#define WMEV_NONE 0
#define WMEV_MOUSE_X 1
#define WMEV_MOUSE_Y 2
#define WMEV_MOUSE_Z 3
#define WMEV_CURSOR_X 4
#define WMEV_CURSOR_Y 5
#define WMEV_KEYDOWN 6
#define WMEV_KEYUP 7
#define WMEV_BUTTONS 8
#define WMEV_DEACTIVATE 9


struct wmmsg
{
	unsigned int window_id;
	int cmd;
	short int x;
	short int y;
	short int w;
	short int h;
	struct wmev ev[256];
	unsigned int nev;
	unsigned int data_size;
	unsigned long seq;
};

#define WMCMD_CREATE_WINDOW 0x7700
#define WMCMD_DELETE_WINDOW 0x7701
#define WMCMD_PAINT_WINDOW 0x7704
#define WMCMD_GET_SCREEN_SIZE 0x7706
#define WMCMD_GET_WINDOW_SIZE 0x7707
#define WMCMD_HIDE_WINDOW 0x7708
#define WMCMD_RESIZE_WINDOW 0x7709
#define WMCMD_READ_EVENT 0x770b

#define MAX_WINDOWS 8

#ifndef _WM_SERVER_

#include "malloc.c"
#include "sock_read.c"
#include "mem.c"
#include "lock.c"
unsigned int wmcall_lock;
int wmfd,wm_connected;
unsigned int screen_size[2];
unsigned int *wmshm;
unsigned long int wmshm_size;
int wm_connect(void)
{
	struct sockaddr_un addr;
	char shm_key[96];
	int fd;
	mutex_lock(&wmcall_lock);
	if(wm_connected)
	{
		mutex_unlock(&wmcall_lock);
		return 1;
	}
	wmfd=socket(AF_UNIX,SOCK_STREAM|SOCK_CLOEXEC,0);
	if(wmfd<0)
	{
		mutex_unlock(&wmcall_lock);
		return 1;
	}
	addr.family=AF_UNIX;
	strcpy(addr.sun_path,"/tmp/windowm.socket");
	if(connect(wmfd,&addr,sizeof(addr)))
	{
		close(wmfd);
		wmfd=-1;
		mutex_unlock(&wmcall_lock);
		return 1;
	}
	sock_read(wmfd,shm_key,96);
	fd=open(shm_key,02000002,0);
	if(fd<0)
	{
		close(wmfd);
		wmfd=-1;
		mutex_unlock(&wmcall_lock);
		return 1;
	}
	wmshm_size=lseek(fd,0,2);
	if(!valid(wmshm_size))
	{
		close(fd);
		close(wmfd);
		wmfd=-1;
		mutex_unlock(&wmcall_lock);
		return 1;
	}
	wmshm=mmap(0,wmshm_size,3,1,fd,0);
	close(fd);
	memset(shm_key,0,96);
	if(!valid(wmshm))
	{
		close(wmfd);
		wmfd=-1;
		mutex_unlock(&wmcall_lock);
		return 1;
	}

	wm_connected=1;
	mutex_unlock(&wmcall_lock);
	return 0;
}
/*
struct wmmsg_list
{
	struct wmmsg buf;
	struct wmmsg_list *next;
};
void wmcall(struct wmmsg *req,void *append,unsigned int size)
{
	static unsigned long seq;
	static unsigned int lock,lock2,lock3;
	static struct wmmsg_list *list,*unused;
	struct wmmsg_list *node,*p;
	struct wmmsg buf;
	int s,s1;
	if(!wm_connected)
	{
		req->cmd=0xfffff001;
		return;
	}
	mutex_lock(&lock);
	++seq;
	req->seq=seq;
	write(wmfd,req,sizeof(*req));
	if(size)
	{
		write(wmfd,append,size);
	}
	mutex_unlock(&lock);
	s1=0;
	do
	{
		mutex_lock(&lock2);
		node=list;
		p=NULL;
		s=0;
		while(node)
		{
			if(node->buf.seq==req->seq)
			{
				memcpy(req,&node->buf,sizeof(*req));
				if(p)
				{
					p->next=node->next;
				}
				else
				{
					list=node->next;
				}
				node->next=unused;
				unused=node;
				s=1;
				break;
			}
			p=node;
			node=node->next;
		}
		if(s1)
		{
			if(unused)
			{
				node=unused;
				unused=node->next;
			}
			else
			{
				node=malloc(sizeof(*node));
				if(node==NULL)
				{
					exit(1);
				}
			}
			memcpy(&node->buf,&buf,sizeof(buf));
			node->next=list;
			list=node;
		}
		mutex_unlock(&lock2);
		if(s)
		{
			return;
		}
		mutex_lock(&lock3);
		sock_read(wmfd,&buf,sizeof(buf));
		mutex_unlock(&lock3);
		if(buf.seq==req->seq)
		{
			memcpy(req,&buf,sizeof(buf));
			return;
		}
		s1=1;
	}
	while(1);
}
*/
void wmcall(struct wmmsg *req,void *append,unsigned int size)
{
	static unsigned long seq;
	int s,s1;
	mutex_lock(&wmcall_lock);
	if(!wm_connected)
	{
		req->cmd=0xfffff001;
		mutex_unlock(&wmcall_lock);
		return;
	}
	++seq;
	if(size)
	{
		if(size>wmshm_size)
		{
			size=wmshm_size;
		}
		memcpy(wmshm,append,size);
	}
	req->seq=seq;
	req->data_size=size;
	write(wmfd,req,sizeof(*req));
	sock_read(wmfd,req,sizeof(*req));
	mutex_unlock(&wmcall_lock);
}
int wm_create_window(int x,int y,int w,int h)
{
	struct wmmsg req;
	req.cmd=WMCMD_CREATE_WINDOW;
	req.x=x;
	req.y=y;
	req.w=w;
	req.h=h;
	wmcall(&req,NULL,0);
	if(req.cmd<0)
	{
		return -1;
	}
	return req.window_id;
}
void wm_delete_window(int window_id)
{
	struct wmmsg req;
	req.cmd=WMCMD_DELETE_WINDOW;
	req.window_id=window_id;
	wmcall(&req,NULL,0);
}
void wm_paint_window(int window_id,unsigned int *buf)
{
	struct wmmsg req;
	int size;
	req.cmd=WMCMD_GET_WINDOW_SIZE;
	req.window_id=window_id;
	wmcall(&req,NULL,0);
	if(req.cmd<0)
	{
		return;
	}
	do
	{
		req.cmd=WMCMD_PAINT_WINDOW;
		size=req.w;
		size*=req.h;
		wmcall(&req,buf,size*4);
	}
	while(req.cmd);
}
void wm_get_screen_size(void)
{
	struct wmmsg req;
	req.cmd=WMCMD_GET_SCREEN_SIZE;
	wmcall(&req,NULL,0);
	screen_size[0]=req.w;
	screen_size[1]=req.h;
}
int wm_hide_window(int window_id)
{
	struct wmmsg req;
	req.cmd=WMCMD_HIDE_WINDOW;
	req.window_id=window_id;
	wmcall(&req,NULL,0);
	return req.cmd;
}
int wm_resize_window(int window_id,int x,int y,int w,int h)
{
	struct wmmsg req;
	req.cmd=WMCMD_RESIZE_WINDOW;
	req.window_id=window_id;
	req.x=x;
	req.y=y;
	req.w=w;
	req.h=h;
	wmcall(&req,NULL,0);
	return req.cmd;
}

#endif

#endif
