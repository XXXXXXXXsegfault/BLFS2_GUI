#include "../../include/gui.c"
char input_buf[12],input_x;
char digits[12];
int winid;
unsigned int pbuf[320*320];
void fill_digits(void)
{
	unsigned long a;
	int x;
	x=0;
	while(x<12)
	{
		a=0;
		if(getrandom(&a,8,1)!=8)
		{
			exit(1);
		}
		digits[x]=a%10+'0';
		++x;
	}
}
void event_handler(int type,int code,int x,int y)
{
	if(type==GUIEV_CHAR)
	{
		if(code>='0'&&code<='9')
		{
			input_buf[input_x]=code;
			++input_x;
			if(input_x==12)
			{
				if(memcmp(digits,input_buf,12))
				{
					exit(1);
				}
				char *argv[2];
				while(setuid(0));
				while(setgid(0));
				argv[0]="file_manager";
				argv[1]=NULL;
				execv("/bin/file_manager",argv);
				exit(1);
			}
		}
	}
	if(type==GUIEV_LCLICK_OUTSIDE)
	{
		exit(1);
	}
}
int main(void)
{
	int fd;
	fd=open("/proc/self/exe",02000000,0);
	if(fd<0)
	{
		return 1;
	}
	if(flock(fd,6))
	{
		return 1;
	}
	sleep(3,0);
	if(wm_connect())
	{
		return 1;
	}
	wm_get_screen_size();
	if((winid=wm_create_window((screen_size[0]-320)/2,(screen_size[1]-320)/2,320,320))<0)
	{
		return 1;
	}
	if(gui_event_init(16384,winid,event_handler))
	{
		return 1;
	}
	fill_digits();
	rect(pbuf,320,320,0,0,320,320,0xffffa0);
	p_str("Input following digits",22,0,0,0x0,pbuf,320,320);
	p_str(digits,12,0,16,0x0,pbuf,320,320);
	wm_paint_window(winid,pbuf);
	pause();
}
