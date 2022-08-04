#ifndef _GUI_C_
#define _GUI_C_

#include "window_manager.c"
#include "mem.c"
#include "lwp.c"
#include "rect.c"
#include "font.c"
#include "signal.c"



struct gui_ev_arg
{
	int window_id;
	int mutex;
	void (*handler)(int,int,int,int); // void handler(int type,int code,int x,int y)
};
#define GUIEV_LCLICK 0x100
#define GUIEV_RCLICK 0x101
#define GUIEV_CHAR 0x102
#define GUIEV_LCLICK_OUTSIDE 0x103
#define GUIEV_RCLICK_OUTSIDE 0x104
#define GUIEV_LDOWN 0x105
#define GUIEV_RDOWN 0x106
#define GUIEV_LUP 0x107
#define GUIEV_RUP 0x108
int translate_key(int code,int shift)
{
	static int tab_init,tab[2][256];
	if(!tab_init)
	{
		memset(tab,0xff,sizeof(tab));
		tab[0][2]='1';
		tab[0][3]='2';
		tab[0][4]='3';
		tab[0][5]='4';
		tab[0][6]='5';
		tab[0][7]='6';
		tab[0][8]='7';
		tab[0][9]='8';
		tab[0][10]='9';
		tab[0][11]='0';
		tab[0][12]='-';
		tab[0][13]='=';
		tab[0][15]='\t';
		tab[0][16]='q';
		tab[0][17]='w';
		tab[0][18]='e';
		tab[0][19]='r';
		tab[0][20]='t';
		tab[0][21]='y';
		tab[0][22]='u';
		tab[0][23]='i';
		tab[0][24]='o';
		tab[0][25]='p';
		tab[0][26]='[';
		tab[0][27]=']';
		tab[0][28]='\n';
		tab[0][30]='a';
		tab[0][31]='s';
		tab[0][32]='d';
		tab[0][33]='f';
		tab[0][34]='g';
		tab[0][35]='h';
		tab[0][36]='j';
		tab[0][37]='k';
		tab[0][38]='l';
		tab[0][39]=';';
		tab[0][40]='\'';
		tab[0][41]='`';
		tab[0][43]='\\';
		tab[0][44]='z';
		tab[0][45]='x';
		tab[0][46]='c';
		tab[0][47]='v';
		tab[0][48]='b';
		tab[0][49]='n';
		tab[0][50]='m';
		tab[0][51]=',';
		tab[0][52]='.';
		tab[0][53]='/';
		tab[0][57]=' ';
		tab[1][2]='!';
		tab[1][3]='@';
		tab[1][4]='#';
		tab[1][5]='$';
		tab[1][6]='%';
		tab[1][7]='^';
		tab[1][8]='&';
		tab[1][9]='*';
		tab[1][10]='(';
		tab[1][11]=')';
		tab[1][12]='_';
		tab[1][13]='+';
		tab[1][15]='\t';
		tab[1][16]='Q';
		tab[1][17]='W';
		tab[1][18]='E';
		tab[1][19]='R';
		tab[1][20]='T';
		tab[1][21]='Y';
		tab[1][22]='U';
		tab[1][23]='I';
		tab[1][24]='O';
		tab[1][25]='P';
		tab[1][26]='{';
		tab[1][27]='}';
		tab[1][28]='\n';
		tab[1][30]='A';
		tab[1][31]='S';
		tab[1][32]='D';
		tab[1][33]='F';
		tab[1][34]='G';
		tab[1][35]='H';
		tab[1][36]='J';
		tab[1][37]='K';
		tab[1][38]='L';
		tab[1][39]=':';
		tab[1][40]='\"';
		tab[1][41]='~';
		tab[1][43]='|';
		tab[1][44]='Z';
		tab[1][45]='X';
		tab[1][46]='C';
		tab[1][47]='V';
		tab[1][48]='B';
		tab[1][49]='N';
		tab[1][50]='M';
		tab[1][51]='<';
		tab[1][52]='>';
		tab[1][53]='?';
		tab[1][57]=' ';
		tab_init=1;
	}
	if(code<0||code>256)
	{
		return -1;
	}
	if(shift)
	{
		return tab[1][code];
	}
	else
	{
		return tab[0][code];
	}
}
int T_gui_evreciever(void *arg)
{
	struct gui_ev_arg *ev;
	int win_id;
	void (*handler)(int,int,int,int);
	struct wmmsg req;
	int x;
	int mouse_x,mouse_y;
	int shift_key,code;
	int lpx,lpy;
	int rpx,rpy;
	int old_button,button;
	ev=arg;
	win_id=ev->window_id;
	handler=ev->handler;
	mutex_unlock(&ev->mutex);
	mouse_x=-1;
	mouse_y=-1;
	lpx=-1;
	lpy=-1;
	rpx=-1;
	rpy=-1;
	old_button=0;
	shift_key=0;
	while(1)
	{
		req.cmd=WMCMD_READ_EVENT;
		req.window_id=win_id;
		wmcall(&req,NULL,0);
		if(req.cmd==0)
		{
			x=0;
			while(x<req.nev)
			{
				handler(req.ev[x].type,req.ev[x].code,req.x,req.y);
				if(req.ev[x].type==WMEV_DEACTIVATE)
				{
					lpx=-1;
					lpy=-1;
					rpx=-1;
					rpy=-1;
					old_button=0;
					mouse_x=-1;
					mouse_y=-1;
					shift_key=0;
				}
				else if(req.ev[x].type==WMEV_BUTTONS)
				{
					button=req.ev[x].code;
					if(button&~old_button&1)
					{
						lpx=mouse_x;
						lpy=mouse_y;
						handler(GUIEV_LDOWN,mouse_x|mouse_y<<16,req.x,req.y);
					}
					if(button&~old_button&2)
					{
						rpx=mouse_x;
						rpy=mouse_y;
						handler(GUIEV_RDOWN,mouse_x|mouse_y<<16,req.x,req.y);
					}
					if(~button&old_button&1)
					{
						if(lpx>=0&&lpy>=0&&lpx<req.w&&lpy<req.h&&
						mouse_x>=lpx-30&&mouse_x<=lpx+30&&
						mouse_y>=lpy-30&&mouse_y<=lpy+30&&
						mouse_x>=0&&mouse_y>=0&&mouse_x<req.w&&mouse_y<req.h)
						{
							handler(GUIEV_LCLICK,mouse_x|mouse_y<<16,req.x,req.y);
						}
						else
						{
							handler(GUIEV_LCLICK_OUTSIDE,0,req.x,req.y);
						}
						lpx=-1;
						lpy=-1;
						handler(GUIEV_LUP,mouse_x|mouse_y<<16,req.x,req.y);
					}
					if(~button&old_button&2)
					{
						if(rpx>=0&&rpy>=0&&rpx<req.w&&rpy<req.h&&
						mouse_x>=rpx-30&&mouse_x<=rpx+30&&
						mouse_y>=rpy-30&&mouse_y<=rpy+30&&
						mouse_x>=0&&mouse_y>=0&&mouse_x<req.w&&mouse_y<req.h)
						{
							handler(GUIEV_RCLICK,mouse_x|mouse_y<<16,req.x,req.y);
						}
						else
						{
							handler(GUIEV_RCLICK_OUTSIDE,0,req.x,req.y);
						}
						rpx=-1;
						rpy=-1;
						handler(GUIEV_RUP,mouse_x|mouse_y<<16,req.x,req.y);
					}
					old_button=button;
				}
				else if(req.ev[x].type==WMEV_CURSOR_X)
				{
					mouse_x=req.ev[x].code-req.x;
				}
				else if(req.ev[x].type==WMEV_CURSOR_Y)
				{
					mouse_y=req.ev[x].code-req.y;
				}
				else if(req.ev[x].type==WMEV_KEYDOWN)
				{
					if(req.ev[x].code==42)
					{
						shift_key|=1;
					}
					else if(req.ev[x].code==54)
					{
						shift_key|=2;
					}
					else
					{
						code=translate_key(req.ev[x].code,shift_key);
						if(code!=-1)
						{
							handler(GUIEV_CHAR,code,req.x,req.y);
						}
					}
				}
				else if(req.ev[x].type==WMEV_KEYUP)
				{
					if(req.ev[x].code==42)
					{
						shift_key&=2;
					}
					else if(req.ev[x].code==54)
					{
						shift_key&=1;
					}
				}
				++x;
			}
		}
		else
		{
			return 1;
		}
		sleep(0,1000);
	}
}
int gui_event_init(int stack_size,int window_id,void (*handler)(int,int,int,int))
{
	struct gui_ev_arg arg;
	arg.window_id=window_id;
	arg.handler=handler;
	arg.mutex=1;
	if(!valid(create_lwp(stack_size,T_gui_evreciever,&arg)))
	{
		return -1;
	}
	mutex_lock(&arg.mutex);
	return 0;
}
int system(char *cmd)
{
	int pid,status;
	pid=fork();
	if(pid==0)
	{
		char *argv[3];
		signal(SIGCHLD,SIG_DFL);
		argv[0]="exec";
		argv[1]=cmd;
		argv[2]=NULL;
		execv("/bin/exec",argv);
		exit(-1);
	}
	if(pid>0)
	{
		status=0;
		waitpid(pid,&status,0);
		return status;
	}
	return pid;
}
int system_nonblock(char *cmd)
{
	int pid;
	pid=fork();
	if(pid==0)
	{
		char *argv[3];
		signal(SIGCHLD,SIG_DFL);
		argv[0]="exec";
		argv[1]=cmd;
		argv[2]=NULL;
		execv("/bin/exec",argv);
		exit(-1);
	}
	return pid;
}
int T_system_nonblock(void *arg)
{
	system(arg);
	free(arg);
	return 0;
}
int system_nonblock2(char *cmd)
{
	int pid;
	char *buf;
	buf=malloc(strlen(cmd)+1);
	if(buf==NULL)
	{
		return -12;
	}
	strcpy(buf,cmd);
	pid=create_lwp(4096,T_system_nonblock,buf);
	if(pid<0)
	{
		free(buf);
		return pid;
	}
	return 0;
}

#endif
