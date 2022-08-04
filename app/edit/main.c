#include "../../include/gui.c"
#include "../../include/mem.c"
#include "../../include/iformat.c"
#include "../../include/stat.c"
int CW,CH;
#define WINW (CW*8+2)
#define WINH (CH*16+25+16)
int winid;
unsigned int *pbuf;
int tmpfd;
char *file_name;
int current_x;

#include "file.c"
struct file_pos select_pos;
#include "undo.c"
#include "cmd.c"
#include "main_window.c"
int init_tmp(void)
{
	char buf[64];
	strcpy(buf,"/tmp/UID");
	sprinti(buf,getuid(),1);
	mkdir(buf,0700);
	tmpfd=open(buf,02000000,0);
	if(tmpfd<0)
	{
		return 1;
	}
	return 0;
}
int main(int argc,char **argv)
{
	struct stat st;
	if(argc<2)
	{
		return 1;
	}
	file_name=argv[1];
	if(stat(argv[1],&st)||(st.mode&0170000)!=0100000)
	{
		return 1;
	}
	if(init_tmp())
	{
		return 1;
	}
	if(file_load())
	{
		return 1;
	}
	if(wm_connect())
	{
		return 1;
	}
	wm_get_screen_size();
	CW=screen_size[0]/12;
	CH=screen_size[1]/24;
	pbuf=malloc(WINW*WINH*4);
	if(pbuf==NULL)
	{
		return 1;
	}
	if((winid=wm_create_window((screen_size[0]-WINW)/2,(screen_size[1]-WINH)/2,WINW,WINH))<0)
	{
		return 1;
	}
	if(gui_event_init(16384,winid,event_handler))
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
