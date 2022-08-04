int dialog_mode;
#define DIALOG_NONE 0
#define DIALOG_RCMENU 1
#define DIALOG_INPUT 2
#define DIALOG_MSG_YESNO 3
#define DIALOG_STATUS 4
#define DIALOG_FMENU 5

int input_type;
#define INPUT_PASTE 1
#define INPUT_PERM 2
#define INPUT_CFILE 3
#define INPUT_CDIR 4
#define INPUT_CMD 5

// input_buf defined in files.c
int input_x,input_y,input_valid;
unsigned int input_lock;
unsigned int dialog_lock;

int if_input_valid(void)
{
	int x;
	char *str,*str2;
	x=0;
	if(input_type==INPUT_PASTE||input_type==INPUT_CFILE||input_type==INPUT_CDIR)
	{
		if(!input_buf[0])
		{
			return 0;
		}
		while(input_buf[x])
		{
			if(input_buf[x]=='/')
			{
				return 0;
			}
			if(x==256)
			{
				return 0;
			}
			++x;
		}
		return 1;
	}
	else if(input_type==INPUT_PERM)
	{
		if(strlen(input_buf)!=9)
		{
			return 0;
		}
		str="rwxrwxrwx";
		str2="  s  s  t";
		x=0;
		while(x<9)
		{
			if(input_buf[x]!='-'&&input_buf[x]!=str[x])
			{
				if(str2[x]==' ')
				{
					return 0;
				}
				else if(input_buf[x]!=str2[x]&&input_buf[x]!=str2[x]-0x20)
				{
					return 0;
				}
			}
			++x;
		}
		return 1;
	}
	else if(input_type==INPUT_CMD)
	{
		if(!input_buf[0])
		{
			return 0;
		}
		return 1;
	}
	return 0;
}
void paint_input_title(void)
{
	if(input_type==INPUT_PASTE)
	{
		p_str("Paste",5,4,4,0x0,dbuf,320,320);
	}
	else if(input_type==INPUT_CFILE)
	{
		p_str("File Name",9,4,4,0x0,dbuf,320,320);
	}
	else if(input_type==INPUT_CDIR)
	{
		p_str("Directory Name",14,4,4,0x0,dbuf,320,320);
	}
	else if(input_type==INPUT_PERM)
	{
		p_str("File Mode (Format: rwxr-xr-x)",29,4,4,0x0,dbuf,320,320);
	}
	else if(input_type==INPUT_CMD)
	{
		p_str("Execute",7,4,4,0x0,dbuf,320,320);
	}
}
void paint_input_box(void)
{
	int x,y,i;
	char ch;
	mutex_lock(&input_lock);
	rect(dbuf,320,320,0,0,320,320,0xffffb0);
	rect(dbuf,320,320,0,0,320,1,0x0);
	rect(dbuf,320,320,0,0,1,320,0x0);
	rect(dbuf,320,320,0,319,320,1,0x0);
	rect(dbuf,320,320,319,0,1,320,0x0);
	paint_input_title();
	y=24;
	x=4;
	if(input_y>input_x)
	{
		input_y=input_x;
	}
	if(input_y<=input_x-39*16)
	{
		input_y=input_x-39*16+1;
	}
	i=input_y;
	while(y+16<288)
	{
		ch=input_buf[i];
		if(ch==0)
		{
			break;
		}
		p_char(ch,x,y,0x0,dbuf,320,320);
		if(i==input_x)
		{
			rect(dbuf,320,320,x,y+14,8,2,0x606060);
		}
		x+=8;
		if(x>=308)
		{
			y+=16;
			x=4;
		}
		++i;
	}
	if(i==input_x)
	{
		rect(dbuf,320,320,x,y+14,8,2,0x606060);
	}
	if(if_input_valid())
	{
		rect(dbuf,320,320,4,292,24,24,0xff0000);
		p_str("OK",2,8,296,0xffffff,dbuf,320,320);
	}
	else
	{
		rect(dbuf,320,320,4,292,24,24,0x404040);
		p_str("OK",2,8,296,0xa0a0a0,dbuf,320,320);
	}
	rect(dbuf,320,320,260,292,56,24,0xff0000);
	p_str("Cancel",6,264,296,0xffffff,dbuf,320,320);
	mutex_unlock(&input_lock);
}
void paint_msg_yesno(void)
{
	int x,y,i;
	char ch;
	mutex_lock(&input_lock);
	rect(dbuf,320,320,0,0,320,320,0xffffb0);
	rect(dbuf,320,320,0,0,320,1,0x0);
	rect(dbuf,320,320,0,0,1,320,0x0);
	rect(dbuf,320,320,0,319,320,1,0x0);
	rect(dbuf,320,320,319,0,1,320,0x0);
	y=24;
	x=4;
	if(input_y>input_x)
	{
		input_y=input_x;
	}
	if(input_y<=input_x-39*16)
	{
		input_y=input_x-39*16+1;
	}
	i=input_y;
	while(y+16<288)
	{
		ch=input_buf[i];
		if(ch==0)
		{
			break;
		}
		p_char(ch,x,y,0x0,dbuf,320,320);
		x+=8;
		if(x>=308)
		{
			y+=16;
			x=4;
		}
		++i;
	}
	rect(dbuf,320,320,4,292,32,24,0xff0000);
	p_str("Yes",3,8,296,0xffffff,dbuf,320,320);
	rect(dbuf,320,320,292,292,24,24,0xff0000);
	p_str("No",2,296,296,0xffffff,dbuf,320,320);
	mutex_unlock(&input_lock);
}
void p_text(char *str)
{
	int x,y,i;
	i=0;
	x=4;
	y=4;
	while(str[i])
	{
		if(str[i]=='\n')
		{
			x=4;
			y+=16;
		}
		else
		{
			p_char(str[i],x,y,0x0,dbuf,320,480);
			x+=8;
			if(x>312)
			{
				x=4;
				y+=16;
			}
		}
		++i;
	}
}
void paint_status(void)
{
	char *str;
	unsigned int mode,x;
	static char buf[512];
	static char buf2[2048];
	buf2[0]=0;
	str="xwrxwrxwr";
	mode=file_selected->lst.mode;
	x=0;
	strcpy(buf,"Mode: ----------\n");
	while(x<9)
	{
		if(mode&1<<x)
		{
			buf[15-x]=str[x];
		}
		++x;
	}
	if(mode&1<<9)
	{
		if(buf[15]=='-')
		{
			buf[15]='T';
		}
		else
		{
			buf[15]='t';
		}
	}
	else if(mode&1<<10)
	{
		if(buf[12]=='-')
		{
			buf[12]='S';
		}
		else
		{
			buf[12]='s';
		}
	}
	else if(mode&1<<11)
	{
		if(buf[9]=='-')
		{
			buf[9]='S';
		}
		else
		{
			buf[9]='s';
		}
	}
	strcpy(buf2,buf);
	strcat(buf2,"Size: ");
	sprinti(buf2,file_selected->lst.size,1);
	strcat(buf2," (");
	size_to_str(file_selected->lst.size,buf);
	strcat(buf2,buf);
	strcat(buf2,")\nSpace Used: ");
	sprinti(buf2,file_selected->lst.blocks*512,1);
	strcat(buf2," (");
	size_to_str(file_selected->lst.blocks*512,buf);
	strcat(buf2,buf);
	strcat(buf2,")\nUID: ");
	sprinti(buf2,file_selected->lst.uid,1);
	strcat(buf2,"\nGID: ");
	sprinti(buf2,file_selected->lst.gid,1);
	strcat(buf2,"\nLast Access: ");
	time_to_str(file_selected->lst.atime,buf);
	strcat(buf2,buf);
	strcat(buf2,"\nLast Modification: ");
	time_to_str(file_selected->lst.mtime,buf);
	strcat(buf2,buf);
	strcat(buf2,"\nName: ");
	strcat(buf2,file_selected->name);
	if((x=readlink(file_selected->name,buf,511))>0)
	{
		strcat(buf2,"\nLink Target: ");
		buf[x]=0;
		strcat(buf2,buf);
	}
	rect(dbuf,320,480,0,0,320,480,0xffffb0);
	rect(dbuf,320,480,0,0,320,1,0x0);
	rect(dbuf,320,480,0,0,1,480,0x0);
	rect(dbuf,320,480,0,479,320,1,0x0);
	rect(dbuf,320,480,319,0,1,480,0x0);
	p_text(buf2);
	rect(dbuf,320,480,148,452,24,24,0xff0000);
	p_str("OK",2,152,456,0xffffff,dbuf,320,480);
}
void set_dialog_mode(int mode,int x,int y)
{
	int w,h,l,paint;
	struct wmmsg req;
	req.cmd=WMCMD_GET_WINDOW_SIZE;
	req.window_id=winid;
	wmcall(&req,NULL,0);
	if(req.cmd)
	{
		return;
	}
	paint=0;
	mutex_lock(&dialog_lock);
	if(mode==DIALOG_NONE)
	{
		mutex_lock(&input_lock);
		input_valid=0;
		mutex_unlock(&input_lock);
		wm_hide_window(dialogid);
	}
	else if(mode==DIALOG_RCMENU)
	{
		if(!req.cmd)
		{
			w=0;
			h=0;
			while(rcmenu_entries[h])
			{
				l=strlen(rcmenu_entries[h]);
				if(l>w)
				{
					w=l;
				}
				++h;
			}
			h*=24;
			w=(w+1)*8;
			if(wm_resize_window(dialogid,x+req.x,y+req.y,w,h))
			{
				mutex_unlock(&dialog_lock);
				return;
			}
			rect(dbuf,w,h,0,0,w,h,0xffffb0);
			l=0;
			while(rcmenu_entries[l])
			{
				p_str(rcmenu_entries[l],strlen(rcmenu_entries[l]),4,l*24+4,0x0,dbuf,w,h);
				rect(dbuf,w,h,0,l*24,w,1,0x0);
				rect(dbuf,w,h,0,l*24,1,24,0x0);
				rect(dbuf,w,h,w-1,l*24,1,24,0x0);
				++l;
			}
			rect(dbuf,w,h,0,h-1,w,1,0x0);
			paint=1;
		}
	}
	else if(mode==DIALOG_INPUT)
	{
		if(!req.cmd)
		{
			if(wm_resize_window(dialogid,x+req.x,y+req.y,320,320))
			{
				mutex_unlock(&dialog_lock);
				return;
			}
			paint_input_box();
			input_valid=1;
			paint=1;
		}
	}
	else if(mode==DIALOG_MSG_YESNO)
	{
		if(!req.cmd)
		{
			if(wm_resize_window(dialogid,x+req.x,y+req.y,320,320))
			{
				mutex_unlock(&dialog_lock);
				return;
			}
			paint_msg_yesno();
			paint=1;
		}
	}
	else if(mode==DIALOG_STATUS)
	{
		if(!req.cmd)
		{
			if(wm_resize_window(dialogid,x+req.x,y+req.y,320,480))
			{
				mutex_unlock(&dialog_lock);
				return;
			}
			paint_status();
			paint=1;
		}
	}
	else if(mode==DIALOG_FMENU)
	{
		if(!req.cmd)
		{
			if(wm_resize_window(dialogid,x+req.x,y+req.y,80,48))
			{
				mutex_unlock(&dialog_lock);
				return;
			}
			rect(dbuf,80,48,0,0,80,48,0xffffb0);
			rect(dbuf,80,48,0,0,80,1,0x0);
			rect(dbuf,80,48,0,0,1,48,0x0);
			rect(dbuf,80,48,0,47,80,1,0x0);
			rect(dbuf,80,48,79,0,1,48,0x0);
			rect(dbuf,80,48,0,24,80,1,0x0);
			p_str("File",4,4,4,0x0,dbuf,80,48);
			p_str("Directory",9,4,28,0x0,dbuf,80,48);
			paint=1;
		}
	}
	if(paint)
	{
		wm_paint_window(dialogid,dbuf);
	}
	dialog_mode=mode;
	mutex_unlock(&dialog_lock);
}
void input_ok_click(void)
{
	unsigned int perm;
	int x;
	mutex_lock(&input_lock);
	if(input_type==INPUT_PASTE)
	{
		create_lwp(8192,T_paste_file,input_buf);
	}
	else if(input_type==INPUT_PERM)
	{
		perm=0;
		x=0;
		while(x<9)
		{
			if(input_buf[x]!='-'&&input_buf[x]!='S'&&input_buf[x]!='T')
			{
				perm|=1<<8-x;
			}
			if(input_buf[x]=='S'||input_buf[x]=='T')
			{
				perm|=0x200<<2-x/3;
			}
			++x;
		}
		chmod(file_chmod,perm);
	}
	else if(input_type==INPUT_CFILE)
	{
		mknod(input_buf,0100644,0);
	}
	else if(input_type==INPUT_CDIR)
	{
		mkdir(input_buf,0755);
	}
	else if(input_type==INPUT_CMD)
	{
		system_nonblock2(input_buf);
	}
	input_valid=0;
	mutex_unlock(&input_lock);
	set_dialog_mode(DIALOG_NONE,0,0);
}
int T_custom_rcmenu(void *arg)
{
	char *str;
	str=arg;
	system(str);
	return 0;
}
void custom_rcmenu(char *cmd,char *fname)
{
	int l,i,j,name_l;
	char *buf;
	l=1;
	i=0;
	name_l=strlen(fname);
	while(cmd[i])
	{
		if(cmd[i]=='$')
		{
			l+=name_l*2;
		}
		else
		{
			++l;
		}
		++i;
	}
	buf=malloc(l);
	if(buf==NULL)
	{
		return;
	}
	i=0;
	j=0;
	while(cmd[i])
	{
		if(cmd[i]=='$')
		{
			l=0;
			while(fname[l])
			{
				buf[j]='\\';
				buf[j+1]=fname[l];
				++l;
				j+=2;
			}
		}
		else
		{
			buf[j]=cmd[i];
			++j;
		}
		++i;
	}
	buf[j]=0;
	if(create_lwp(4096,T_custom_rcmenu,buf)<0)
	{
		free(buf);
	}
}
void dialog_click(int mouse_x,int mouse_y)
{
	int n,l;
	struct wmmsg req,req2;
	mutex_lock(&dialog_lock);
	if(dialog_mode==DIALOG_RCMENU)
	{
		req.cmd=WMCMD_GET_WINDOW_SIZE;
		req.window_id=dialogid;
		wmcall(&req,NULL,0);
		req2.cmd=WMCMD_GET_WINDOW_SIZE;
		req2.window_id=winid;
		wmcall(&req2,NULL,0);
		n=mouse_y/24;
		mutex_unlock(&dialog_lock);
		set_dialog_mode(DIALOG_NONE,0,0);
		if(n==0) // copy
		{
			set_copy_path(0);
		}
		else if(n==1) // execute
		{
			exec_file();
		}
		else if(n==2) // mode
		{
			if(req.cmd==0&&req2.cmd==0)
			{
				file_chmod_msg();
				if(file_chmod[0])
				{
					mutex_lock(&input_lock);
					input_type=INPUT_PERM;
					input_y=0;
					input_x=0;
					input_buf[0]=0;
					input_valid=1;
					mutex_unlock(&input_lock);
					set_dialog_mode(DIALOG_INPUT,mouse_x+req.x-req2.x,mouse_y+req.y-req2.y);
				}
			}
		}
		else if(n==3) //move
		{
			set_copy_path(1);
		}
		else if(n==4) //remove
		{
			if(req.cmd==0&&req2.cmd==0)
			{
				remove_file_msg();
				if(file_removing[0])
				{
					set_dialog_mode(DIALOG_MSG_YESNO,mouse_x+req.x-req2.x,mouse_y+req.y-req2.y);
				}
			}
		}
		else if(n==5) //status
		{
			if(req.cmd==0&&req2.cmd==0)
			{
				mutex_lock(&files_lock);
				if(file_selected&&!file_selected->lst_valid)
				{
					set_dialog_mode(DIALOG_STATUS,mouse_x+req.x-req2.x,mouse_y+req.y-req2.y);
				}
				mutex_unlock(&files_lock);
			}
		}
		else if(n<12&&rcmenu_entries[n])
		{
			mutex_lock(&files_lock);
			if(file_selected)
			{
				l=strlen(rcmenu_entries[n]);
				custom_rcmenu(rcmenu_entries[n]+l+1,file_selected->name);
			}
			mutex_unlock(&files_lock);
		}
		mutex_lock(&files_lock);
		file_selected=NULL;
		mutex_unlock(&files_lock);
	}
	else if(dialog_mode==DIALOG_INPUT)
	{
		mutex_unlock(&dialog_lock);
		if(mouse_x>=260&&mouse_x<316&&mouse_y>=292&&mouse_y<316)
		{
			set_dialog_mode(DIALOG_NONE,0,0);
			mutex_lock(&input_lock);
			input_valid=0;
			mutex_unlock(&input_lock);
		}
		else if(mouse_x>=4&&mouse_x<28&&mouse_y>=292&&mouse_y<316)
		{
			if(if_input_valid())
			{
				input_ok_click();
			}
		}
	}
	else if(dialog_mode==DIALOG_MSG_YESNO)
	{
		mutex_unlock(&dialog_lock);
		if(mouse_x>=292&&mouse_x<316&&mouse_y>=292&&mouse_y<316)
		{
			set_dialog_mode(DIALOG_NONE,0,0);
		}
		else if(mouse_x>=4&&mouse_x<36&&mouse_y>=292&&mouse_y<316)
		{
			do_remove_file();
			set_dialog_mode(DIALOG_NONE,0,0);
		}
	}
	else if(dialog_mode==DIALOG_STATUS)
	{
		mutex_unlock(&dialog_lock);
		if(mouse_x>=148&&mouse_x<172&&mouse_y>=452&&mouse_y<476)
		{
			set_dialog_mode(DIALOG_NONE,0,0);
		}
	}
	else if(dialog_mode==DIALOG_FMENU)
	{
		mutex_unlock(&dialog_lock);
		mutex_lock(&input_lock);
		if(mouse_y<24)
		{
			input_type=INPUT_CFILE;
		}
		else
		{
			input_type=INPUT_CDIR;
		}
		input_x=0;
		input_y=0;
		input_buf[0]=0;
		input_valid=1;
		mutex_unlock(&input_lock);
		set_dialog_mode(DIALOG_INPUT,WINW-40-48-80+mouse_x,24+mouse_y);
	}
	else
	{
		mutex_unlock(&dialog_lock);
	}
}
void dialog_event_handler(int type,int code,int x,int y)
{
	int input_op;
	int mode;
	input_op=0;
	if(type==WMEV_DEACTIVATE||type==GUIEV_LCLICK_OUTSIDE)
	{
		mutex_lock(&dialog_lock);
		mode=dialog_mode;
		mutex_unlock(&dialog_lock);
		if(mode==DIALOG_RCMENU||mode==DIALOG_FMENU)
		{
			set_dialog_mode(DIALOG_NONE,0,0);
			mutex_lock(&files_lock);
			file_selected=NULL;
			mutex_unlock(&files_lock);
			paint=1;
		}
		return;
	}
	if(type==GUIEV_LCLICK)
	{
		dialog_click(code&0xffff,code>>16);
		return;
	}
	if(type==GUIEV_CHAR)
	{
		mutex_lock(&input_lock);
		if(input_valid)
		{
			if(code>=32&&code<=126&&input_x!=16383)
			{
				memmove(input_buf+input_x+1,input_buf+input_x,16384-input_x-1);
				input_buf[input_x]=code;
				++input_x;
				input_op=1;
			}
			else if(code=='\n')
			{
				input_op=2;
			}
		}
		mutex_unlock(&input_lock);
		if(input_op==1)
		{
			mutex_lock(&dialog_lock);
			paint_input_box();
			wm_paint_window(dialogid,dbuf);
			mutex_unlock(&dialog_lock);
		}
		else if(input_op==2)
		{
			if(if_input_valid())
			{
				input_ok_click();
			}
		}
		return;
	}
	if(type==WMEV_KEYDOWN)
	{
		mutex_lock(&input_lock);
		if(input_valid)
		{
			if(code==14&&input_x)
			{
				memmove(input_buf+input_x-1,input_buf+input_x,16384-input_x);
				--input_x;
				input_op=1;
			}
			else if(code==105&&input_x)
			{
				--input_x;
				input_op=1;
			}
			else if(code==106&&input_buf[input_x])
			{
				++input_x;
				input_op=1;
			}
		}
		mutex_unlock(&input_lock);
		if(input_op)
		{
			mutex_lock(&dialog_lock);
			paint_input_box();
			wm_paint_window(dialogid,dbuf);
			mutex_unlock(&dialog_lock);
		}
		return;
	}
}
