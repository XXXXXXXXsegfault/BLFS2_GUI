#include "../../include/malloc.c"
#include "../../include/signal.c"
#include "../../include/gui.c"

#define EXIT_SHUTDOWN 9

#define WINW 302
#define WINH 326

#define BUTTON_X ((WINW-216)/2)
#define BUTTON_Y (WINH-32)

int winid;

unsigned int pbuf[WINW*WINH];
int paint,mode;
int mouse_x,mouse_y;

struct config_entry
{
	char *buf;
	struct config_entry *next;
} *config_entries,*config_entries_end;
void config_load(void)
{
	int fd;
	long size;
	char *config_map;
	char *buf;
	struct config_entry *entry;
	int x,x1,valid;
	fd=open("/etc/main_menu",0,0);
	if(fd<0)
	{
		return;
	}
	size=lseek(fd,0,2);
	if(size<0)
	{
		close(fd);
		return;
	}
	config_map=mmap(0,size,1,1,fd,0);
	if(!valid(config_map))
	{
		close(fd);
		return;
	}
	x=0;
	x1=0;
	valid=0;
	while(x<size)
	{
		if(config_map[x]==':')
		{
			valid=1;
		}
		else if(config_map[x]=='\n')
		{
			if(valid&&x>=x1+3)
			{
				entry=malloc(sizeof(*entry));
				if(entry)
				{
					buf=malloc(x-x1+1);
					if(buf)
					{
						memcpy(buf,config_map+x1,x-x1);
						buf[x-x1]=0;
						entry->buf=buf;
						entry->next=NULL;
						if(config_entries_end)
						{
							config_entries_end->next=entry;
						}
						else
						{
							config_entries=entry;
						}
						config_entries_end=entry;
					}
					else
					{
						free(entry);
					}
				}
			}
			x1=x+1;
			valid=0;
		}
		++x;
	}
	munmap(config_map,size);
}
void show_entries(void)
{
	int x,y,y1;
	struct config_entry *entry;
	unsigned int color;
	char c;
	y=0;
	y1=48;
	entry=config_entries;
	while(y<15&&entry)
	{
		if(mouse_x>=4&&mouse_x<WINW-4&&mouse_y>=y1&&mouse_y<y1+16)
		{
			color=0xa0a0;
		}
		else
		{
			color=0xff;
		}
		x=0;
		while(c=entry->buf[x])
		{
			if(c==':')
			{
				break;
			}
			p_char(c,4+x*8,y1,color,pbuf,WINW,WINH);
			++x;
		}
		++y;
		y1+=16;
		entry=entry->next;
	}
	if(mouse_x>=4&&mouse_x<WINW-4&&mouse_y>=y1&&mouse_y<y1+16)
	{
		color=0xa0a0;
	}
	else
	{
		color=0xff;
	}
	p_str("Exit",4,4,y1,color,pbuf,WINW,WINH);
}
void paint_all(void)
{
	static unsigned int lock;
	unsigned int color[3];
	int x,y;
	mutex_lock(&lock);
	rect(pbuf,WINW,WINH,0,24,WINW,WINH-24,0xffffff);
	if(mode==0)
	{
		show_entries();
		p_str("Press META key to drag windows",30,4,28,0xff0000,pbuf,WINW,WINH);
	}
	else
	{
		p_str("Exit",4,4,28,0xff00,pbuf,WINW,WINH);
		color[0]=0xff;
		color[1]=0xff;
		color[2]=0xff;
		x=mouse_x-BUTTON_X;
		y=mouse_y-BUTTON_Y;
		if(x>=0&&x<216&&y>=0&&y<16)
		{
			color[x/72]=0xa0a0;
		}
		p_str("Cancel",6,BUTTON_X,BUTTON_Y,color[0],pbuf,WINW,WINH);
		p_str("Poweroff",8,BUTTON_X+72,BUTTON_Y,color[1],pbuf,WINW,WINH);
		p_str("Reboot",6,BUTTON_X+144,BUTTON_Y,color[2],pbuf,WINW,WINH);
	}
	rect(pbuf,WINW,WINH,0,0,WINW,24,0x00c0c0);
	rect(pbuf,WINW,WINH,0,0,WINW,1,0x404040);
	rect(pbuf,WINW,WINH,0,0,1,WINH,0x404040);
	rect(pbuf,WINW,WINH,0,WINH-1,WINW,1,0x404040);
	rect(pbuf,WINW,WINH,WINW-1,0,1,WINH,0x404040);
	rect(pbuf,WINW,WINH,0,23,WINW,1,0x404040);
	p_str("Main Menu",9,4,4,0x404040,pbuf,WINW,WINH);
	wm_paint_window(winid,pbuf);
	mutex_unlock(&lock);
}
void click_event(int x,int y)
{
	int y1;
	if(mode==0)
	{
		if(x>=4&&x<WINW-4&&y>=48)
		{
			struct config_entry *entry;
			char *str,c;
			y1=48;
			entry=config_entries;
			while(entry)
			{
				if(y>=y1&&y<y1+16)
				{
					break;
				}
				y1+=16;
				entry=entry->next;
			}
			if(entry)
			{
				str=entry->buf;
				while(c=*str)
				{
					++str;
					if(c==':')
					{
						break;
					}
				}
				if(c)
				{
					system_nonblock(str);
				}
			}
			else if(y<y1+16)
			{
				mode=1;
				paint=1;
			}
		}
	}
	else
	{
		x-=BUTTON_X;
		y-=BUTTON_Y;
		if(x>=0&&x<216&&y>=0&&y<16)
		{
			if(x<72)
			{
				mode=0;
				paint=1;
			}
			else if(x<144)
			{
				exit(EXIT_SHUTDOWN);
			}
			else
			{
				exit(0);
			}
		}
	}
}
void event_handler(int type,int code,int x,int y)
{
	static int old_buttons;
	static int press_x,press_y;
	static int press_pos_valid;
	if(type==WMEV_DEACTIVATE)
	{
		old_buttons=0;
		press_pos_valid=0;
		mouse_x=-1;
		mouse_y=-1;
		paint=1;
		return;
	}
	if(type==WMEV_CURSOR_X)
	{
		mouse_x=code-x;
		paint=1;
	}
	else if(type==WMEV_CURSOR_Y)
	{
		mouse_y=code-y;
		paint=1;
	}
	else if(type==WMEV_BUTTONS)
	{
		if(~old_buttons&code&1)
		{
			press_x=mouse_x;
			press_y=mouse_y;
			press_pos_valid=1;
		}
		else if(old_buttons&~code&1)
		{
			if(press_pos_valid)
			{
				if(mouse_x>=press_x-5&&mouse_x<=press_x+5&&
				mouse_y>=press_y-5&&mouse_y<=press_y+5)
				{
					click_event(mouse_x,mouse_y);
				}
				press_pos_valid=0;
			}
		}
		old_buttons=code;
	}
}
int main(void)
{
	int pid,status;
	int fdi,fdo,fde;
	if(getuid()!=0)
	{
		return 1;
	}
	pid=fork();

	if(pid<0)
	{
		return 1;
	}
	if(pid>0)
	{
		wait(&status);
		if(status==EXIT_SHUTDOWN<<8)
		{
			kill(1,SIGQUIT);
		}
		else
		{
			kill(1,SIGINT);
		}
		return 0;
	}
	fdi=open("/dev/zero",0,0);
	if(fdi<0)
	{
		return 1;
	}
	fdo=open("/dev/zero",1,0);
	if(fdo<0)
	{
		return 1;
	}
	fde=open("/dev/zero",1,0);
	if(fde<0)
	{
		return 1;
	}
	dup2(fdi,0);
	dup2(fdo,1);
	dup2(fde,2);
	close(fdi);
	close(fdo);
	close(fde);

	while(setgid(4000));
	while(setuid(4000));
	signal(SIGCHLD,SIG_IGN);
	chdir("/home");
	if(wm_connect())
	{
		return 1;
	}
	config_load();
	if((winid=wm_create_window(40,40,WINW,WINH))<0)
	{
		return 1;
	}
	mouse_x=-1;
	mouse_y=-1;
	if(gui_event_init(16384,winid,event_handler)<0)
	{
		return 1;
	}
	paint=1;
	while(1)
	{
		if(lock_set32(&paint,0))
		{
			paint_all();
		}
		sleep(0,1000);
	}
}
