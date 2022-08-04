#include "../../include/gui.c"
#include "../../include/signal.c"
#include "../../include/path.c"
#include "../../include/dirent.c"
#include "../../include/mem.c"
#include "../../include/iformat.c"
#define WINW 602
#define WINH 474

#define PATH_H 48
#define FILE_H (WINH-PATH_H-2)

int winid,dialogid;
int paint;
unsigned int pbuf[WINW*WINH];
unsigned int dbuf[WINW*WINH];
char *rcmenu_entries[13];
int tmpfd;

unsigned char conf_buf[1026];


void init_menu_entries(void)
{
	int x,x1,y;
	int fd;
	char c;
	rcmenu_entries[0]="Copy";
	rcmenu_entries[1]="Execute";
	rcmenu_entries[2]="Mode";
	rcmenu_entries[3]="Move";
	rcmenu_entries[4]="Remove";
	rcmenu_entries[5]="Status";
	fd=open("/etc/file_manager/custom_rcmenu",0,0);
	if(fd>=0)
	{
		read(fd,conf_buf,1024);
		close(fd);
	}
	x=0;
	x1=0;
	y=6;
	while(c=conf_buf[x])
	{
		if(c==':')
		{
			conf_buf[x]=0;
		}
		else if(c=='\n')
		{
			conf_buf[x]=0;
			rcmenu_entries[y]=conf_buf+x1;
			x1=x+1;
			++y;
			if(y==12)
			{
				break;
			}
		}
		++x;
	}
}
int init_tmp(void)
{
	char buf[64];
	strcpy(buf,"/tmp/UID");
	sprinti(buf,getuid(),1);
	mkdir(buf,0700);
	strcat(buf,"/file_manager");
	mkdir(buf,0755);
	tmpfd=open(buf,02000000,0);
	if(tmpfd<0)
	{
		return 1;
	}
	return 0;
}
#include "icons.c"
#include "files.c"
#include "dialog.c"
#include "main_window.c"
int T_scan(void *args)
{
	while(1)
	{
		files_scan();
		paint=1;
		sleep(0,50000);
	}
}
int main(void)
{
	if(wm_connect())
	{
		return 1;
	}
	icons_init();
	init_menu_entries();
	if(init_tmp())
	{
		return 1;
	}
	wm_get_screen_size();
	winid=wm_create_window((screen_size[0]-WINW)/2,(screen_size[1]-WINH)/2,WINW,WINH);
	current_wd=getwd();
	if(winid<0)
	{
		return 1;
	}
	dialogid=wm_create_window(0,0,10,10);
	if(dialogid<0)
	{
		return 1;
	}
	if(create_lwp(4096,T_scan,0)<0)
	{
		return 1;
	}
	if(gui_event_init(16384,winid,event_handler))
	{
		return 1;
	}
	if(gui_event_init(16384,dialogid,dialog_event_handler))
	{
		return 1;
	}
	while(1)
	{
		if(lock_set32(&paint,0))
		{
			paint_all();
		}
		sleep(0,1000);
	}
}
