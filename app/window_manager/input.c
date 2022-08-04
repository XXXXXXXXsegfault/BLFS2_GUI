struct input_event
{
	long sec;
	long usec;
	unsigned short type;
	unsigned short code;
	int value;
};
int winkey_pressed;
int T_input_dev(void *arg)
{
	int fd;
	struct input_event event;
	struct pollfd pfd;
	int n;
	fd=(long)arg;
	n=0;
	while(1)
	{
		pfd.fd=fd;
		pfd.events=POLLIN;
		if(poll(&pfd,1,0))
		{
			if(read(fd,&event,sizeof(event))!=sizeof(event))
			{
				close(fd);
				return 0;
			}
			if(event.type==1)
			{
				if(event.code==125) // WIN key
				{
					mutex_lock(&display_lock);
					winkey_pressed=event.value;
					mutex_unlock(&display_lock);
				}
				else if(event.value)
				{
					mutex_lock(&display_lock);
					send_event(WMEV_KEYDOWN,event.code);
					mutex_unlock(&display_lock);
				}
				else
				{
					mutex_lock(&display_lock);
					send_event(WMEV_KEYUP,event.code);
					mutex_unlock(&display_lock);
				}
			}
			else if(event.type==2)
			{
				if(event.code==8)
				{
					mutex_lock(&display_lock);
					send_event(WMEV_MOUSE_Z,event.value);
					mutex_unlock(&display_lock);
				}
			}
			++n;
			if(n==1000)
			{
				sleep(0,4000);
				n=0;
			}
		}
		else
		{
			sleep(0,4000);
			n=0;
		}
	}
}

int T_input(void *args)
{
	int x;
	int fd;
	char buf[32];
	while(1)
	{
		x=0;
		while(x<32)
		{
			strcpy(buf,"/dev/input/event");
			sprinti(buf,x,1);
			fd=open(buf,0,0);
			if(fd>=0)
			{
				if(flock(fd,6))
				{
					close(fd);
				}
				else if(!valid(create_lwp(4096,T_input_dev,(void *)((long)fd))))
				{
					close(fd);
				}
			}
			++x;
		}
		sleep(0,500000);
	}
}
int T_mice(void *args)
{
	int fd;
	char event[3];
	long int x,y,x1,y1;
	char old_button;
	struct pollfd pfd;
	struct window *win;
	int n;
	while((fd=open("/dev/input/mice",0,0))<0)
	{
		sleep(0,100000);
	}
	old_button=0;
	n=0;
	while(1)
	{
		pfd.fd=fd;
		pfd.events=POLLIN;
		if(poll(&pfd,1,0))
		{
			if(read(fd,event,3)==3)
			{
				mutex_lock(&display_lock);
				x=event[1];
				y=event[2];
				y=-y;
				x=x*(long)5/(long)4;
				y=y*(long)5/(long)4;
				x1=cursor_x;
				y1=cursor_y;
				cursor_x+=x;
				cursor_y+=y;
				if(cursor_x<0)
				{
					cursor_x=0;
				}
				if(cursor_y<0)
				{
					cursor_y=0;
				}
				if(cursor_x>=screen_w)
				{
					cursor_x=screen_w-1;
				}
				if(cursor_y>=screen_h)
				{
					cursor_y=screen_h-1;
				}
				set_pz(x1-10,y1-10,21,21);
				set_pz(cursor_x-10,cursor_y-10,21,21);
				send_event(WMEV_CURSOR_X,cursor_x);
				send_event(WMEV_CURSOR_Y,cursor_y);
				send_event(WMEV_MOUSE_X,x);
				send_event(WMEV_MOUSE_Y,y);
				send_event(WMEV_BUTTONS,event[0]&7);
				if(win_moving)
				{
					set_pz(win_moving->x,win_moving->y,win_moving->w,win_moving->h);
					win_moving->x+=cursor_x-x1;
					win_moving->y+=cursor_y-y1;
					set_pz(win_moving->x,win_moving->y,win_moving->w,win_moving->h);
					if(!(event[0]&1))
					{
						win_moving=0;
					}
				}
				if(winkey_pressed)
				{
					if(event[0]&~old_button&1)
					{
						win=win_end;
						while(win)
						{
							x1=cursor_x-win->x;
							y1=cursor_y-win->y;
							if(win->paint&&x1>=0&&y1>=0&&x1<win->w&&y1<win->h)
							{
								break;
							}
							win=win->prev;
						}
						if(win)
						{
							send_event(WMEV_DEACTIVATE,0);
							activate_window(win);
							win_moving=win;
						}
					}
				}
				mutex_unlock(&display_lock);
				old_button=event[0];
				paint=1;
			}
			else
			{
				close(fd);
				while((fd=open("/dev/input/mice",0,0))<0)
				{
					sleep(0,100000);
				}
			}
			++n;
			if(n==1000)
			{
				sleep(0,4000);
				n=0;
			}
		}
		else
		{
			sleep(0,4000);
			n=0;
		}
	}
}
