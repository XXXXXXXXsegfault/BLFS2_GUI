#include "../../include/gui.c"
#include "../../include/iformat.c"
#include "../../include/dirent.c"
#define WINW 610
#define WINH 441
int winid;
unsigned int pbuf[WINW*WINH];
int paint;

#include "proc.c"
int T_proc_scan(void *args)
{
	while(1)
	{
		scan_proc();
		paint=1;
		sleep(2,0);
	}
}
void paint_all(void)
{
	rect(pbuf,WINW,WINH,0,0,WINW,WINH,0xffffff);

	paint_proc();
	rect(pbuf,WINW,WINH,0,0,WINW,24,0xc0c0);
	rect(pbuf,WINW,WINH,WINW-40,0,40,24,0xff0000);
	rect(pbuf,WINW,WINH,WINW-40-40,0,40,24,0xff00);
	p_str("Exit",4,WINW-40+4,4,0xffffff,pbuf,WINW,WINH);
	p_str("Kill",4,WINW-40-40+4,4,0x0,pbuf,WINW,WINH);

	if(getuid()==0)
	{
		p_str("Task Manager (root)",19,4,4,0x404040,pbuf,WINW,WINH);
	}
	else
	{
		p_str("Task Manager",12,4,4,0x404040,pbuf,WINW,WINH);
	}

	rect(pbuf,WINW,WINH,0,0,WINW,1,0x404040);
	rect(pbuf,WINW,WINH,0,0,1,WINH,0x404040);
	rect(pbuf,WINW,WINH,0,WINH-1,WINW,1,0x404040);
	rect(pbuf,WINW,WINH,WINW-1,0,1,WINH,0x404040);
	rect(pbuf,WINW,WINH,0,23,WINW,1,0x404040);

	wm_paint_window(winid,pbuf);
}
void set_sorting_mode(int mode)
{
	mutex_lock(&proc_lock);
	if(mode==(sorting_mode&0xfffffffe))
	{
		sorting_mode^=1;
	}
	else
	{
		sorting_mode=mode;
	}
	mutex_unlock(&proc_lock);
	paint=1;
}
int T_kill(void *arg)
{
	int pid;
	mutex_lock(&proc_lock);
	pid=selected_pid;
	mutex_unlock(&proc_lock);
	if(pid==0)
	{
		return 0;
	}
	if(kill(pid,0))
	{
		return 1;
	}
	kill(pid,SIGINT);
	sleep(0,300000);
	kill(pid,SIGTERM);
	sleep(2,0);
	kill(pid,SIGKILL);
	return 0;
}
void scrolldown(int n)
{
	mutex_lock(&proc_lock);
	current_y+=n;
	mutex_unlock(&proc_lock);
	paint=1;
}
void scrollup(int n)
{
	mutex_lock(&proc_lock);
	current_y-=n;
	mutex_unlock(&proc_lock);
	paint=1;
}
void event_handler(int type,int code,int x,int y)
{
	int mouse_x,mouse_y;
	struct proc *node;
	int y1;
	if(type==WMEV_DEACTIVATE)
	{
		return;
	}
	if(type==GUIEV_LCLICK)
	{
		mouse_x=(short)code;
		mouse_y=code>>16;
		if(mouse_y<24)
		{
			if(mouse_x>=WINW-40)
			{
				exit(0);
			}
			else if(mouse_x>=WINW-40-40) //kill
			{
				create_lwp(4096,T_kill,NULL);
			}
		}
		else if(mouse_y>=40&&mouse_y<56)
		{
			if(mouse_x>=88&&mouse_x<88+32)
			{
				set_sorting_mode(0);
			}
			else if(mouse_x>=88+136&&mouse_x<88+136+24)
			{
				set_sorting_mode(2);
			}
			else if(mouse_x>=88+136+64&&mouse_x<88+136+64+48)
			{
				set_sorting_mode(4);
			}
			else
			{
				mutex_lock(&proc_lock);
				selected_pid=0;
				mutex_unlock(&proc_lock);
				paint=1;
			}
		}
		else if(mouse_y>56)
		{
			mutex_lock(&proc_lock);
			y1=56-current_y;
			node=proc_paint_list;
			while(node)
			{
				if(mouse_y>=y1&&mouse_y<y1+24)
				{
					break;
				}
				node=node->paint_next;
				y1+=24;
			}
			if(node)
			{
				selected_pid=node->pid;
			}
			else
			{
				selected_pid=0;
			}
			mutex_unlock(&proc_lock);
			paint=1;
		}
		else
		{
			mutex_lock(&proc_lock);
			selected_pid=0;
			mutex_unlock(&proc_lock);
			paint=1;
		}
	}
	else if(type==WMEV_KEYDOWN)
	{
		if(code==103)
		{
			scrollup(24);
		}
		else if(code==108)
		{
			scrolldown(24);
		}
	}
	else if(type==WMEV_MOUSE_Z)
	{
		if(code<0)
		{
			scrolldown(-code*24);
		}
		else
		{
			scrollup(code*24);
		}
	}
}
int main(void)
{
	if(wm_connect())
	{
		return 1;
	}
	scan_proc();
	sleep(0,100000);
	wm_get_screen_size();
	if((winid=wm_create_window((screen_size[0]-WINW)/2,(screen_size[1]-WINH)/2,WINW,WINH))<0)
	{
		return 1;
	}
	if(gui_event_init(16384,winid,event_handler))
	{
		return 1;
	}
	if(create_lwp(16384,T_proc_scan,NULL)<0)
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

