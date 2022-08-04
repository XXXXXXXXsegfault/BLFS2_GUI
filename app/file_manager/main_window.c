void paint_all(void)
{
	long int l;
	char *str;
	int scroll_y;
	rect(pbuf,WINW,WINH,0,24,WINW,WINH-24,0xffffff);
	p_files();	
	mutex_lock(&files_lock);
	scroll_y=current_y;
	if(max_y!=0)
	{
		rect(pbuf,WINW,WINH,WINW-1-10,PATH_H,10,WINH-2-PATH_H,0x404040);
		rect(pbuf,WINW,WINH,WINW-10,PATH_H+1+scroll_y*(WINH-2-PATH_H-22)/max_y,8,20,0xa0a0a0);
	}

	rect(pbuf,WINW,WINH,0,24,WINW,24,0xffffc0);
	display_icon(icon_go_back,4,28);
	if(current_wd)
	{
		l=strlen(current_wd);
		if(l>(WINW-42)/8)
		{
			str=current_wd+l-(WINW-42)/8;
		}
		else
		{
			str=current_wd;
		}
		p_str(str,strlen(str),32,28,0x0,pbuf,WINW,WINH);
	}
	mutex_unlock(&files_lock);
	rect(pbuf,WINW,WINH,0,0,WINW,24,0xc0c0);
	rect(pbuf,WINW,WINH,WINW-40,0,40,24,0xff0000);
	rect(pbuf,WINW,WINH,WINW-40-48,0,48,24,0xff00);
	rect(pbuf,WINW,WINH,WINW-40-48-32,0,32,24,0xffff00);
	if(getuid()==0)
	{
		p_str("Files (root)",12,4,4,0x404040,pbuf,WINW,WINH);
	}
	else
	{
		p_str("Files (Press Ctrl-R to run a command)",37,4,4,0x404040,pbuf,WINW,WINH);
	}
	p_str("Exit",4,WINW-40+4,4,0xffffff,pbuf,WINW,WINH);
	p_str("Paste",5,WINW-40-48+4,4,0x0,pbuf,WINW,WINH);
	p_str("New",3,WINW-40-48-32+4,4,0x0,pbuf,WINW,WINH);
	rect(pbuf,WINW,WINH,0,0,WINW,1,0x404040);
	rect(pbuf,WINW,WINH,0,0,1,WINH,0x404040);
	rect(pbuf,WINW,WINH,0,WINH-1,WINW,1,0x404040);
	rect(pbuf,WINW,WINH,WINW-1,0,1,WINH,0x404040);
	rect(pbuf,WINW,WINH,0,23,WINW,1,0x404040);

	wm_paint_window(winid,pbuf);
}
void load_copy_name(void)
{
	int fd;
	long int l,x;
	char *buf;
	input_x=0;
	input_y=0;
	input_buf[0]=0;
	input_type=INPUT_PASTE;
	fd=openat(tmpfd,"copy_src",02000000,0);
	if(fd<0)
	{
		return;
	}
	l=lseek(fd,0,2);
	if(l<0)
	{
		close(fd);
		return;
	}
	lseek(fd,0,0);
	buf=malloc(l);;
	if(buf==NULL)
	{
		close(fd);
		return;
	}
	memset(buf,"\n",l);
	read(fd,buf,l);
	x=0;
	while(x<l)
	{
		if(buf[x]=='\n')
		{
			break;
		}
		++x;
	}
	l=x;
	while(x>0)
	{
		if(buf[x-1]=='/'||buf[x-1]==':')
		{
			break;
		}
		--x;
	}
	memcpy(input_buf,buf+x,l-x);
	input_buf[l-x]=0;
	free(buf);
	close(fd);
	input_x=strlen(input_buf);
}
void caption_click(int mouse_x)
{
	char s;
	if(mouse_x>=WINW-40)
	{
		exit(0);
	}
	else if(mouse_x>=WINW-40-48)
	{
		mutex_lock(&input_lock);
		load_copy_name();
		s=input_buf[0];
		mutex_unlock(&input_lock);
		if(s)
		{
			set_dialog_mode(DIALOG_INPUT,WINW-40-320,24);
		}
	}
	else if(mouse_x>=WINW-40-48-32)
	{
		set_dialog_mode(DIALOG_FMENU,WINW-40-48-80,24);
	}
}
void files_scrolldown(int n)
{
	mutex_lock(&files_lock);
	current_y+=n;
	if(current_y>max_y)
	{
		current_y=max_y;
	}
	mutex_unlock(&files_lock);
	paint=1;
}
void files_scrollup(int n)
{
	mutex_lock(&files_lock);
	current_y-=n;
	if(current_y<0)
	{
		current_y=0;
	}
	mutex_unlock(&files_lock);
	paint=1;
}
void file_click(int mouse_x,int mouse_y)
{
	struct file *node;
	int y;
	if(y>=WINH-1-10)
	{
		return;
	}
	mutex_lock(&files_lock);
	y=PATH_H-current_y;
	node=files;
	while(node)
	{
		if(mouse_y>=y&&mouse_y<y+24)
		{
			break;
		}
		node=node->next;
		y+=24;
	}
	if(node)
	{
		file_selected=node;
		set_dialog_mode(DIALOG_RCMENU,mouse_x,mouse_y);
		paint=1;
	}
	mutex_unlock(&files_lock);
}
void click_event(int mouse_x,int mouse_y)
{
	int y;
	struct file *node;
	char name[256];
	if(mouse_x<1||mouse_x>=WINW-1||mouse_y<1||mouse_y>=WINH-1)
	{
		return;
	}
	if(mouse_y<24)
	{
		caption_click(mouse_x);
		return;
	}
	if(mouse_y<PATH_H)
	{
		if(mouse_x>=4&&mouse_y>=28&&mouse_x<20&&mouse_y<44)
		{
			chdir("..");
			reload_cwd();
			files_scan();
			mutex_lock(&files_lock);
			current_y=0;
			mutex_unlock(&files_lock);
			paint=1;
			set_dialog_mode(DIALOG_NONE,0,0);
		}
		return;
	}
	set_dialog_mode(DIALOG_NONE,0,0);
	mutex_lock(&files_lock);
	node=files;
	y=PATH_H-current_y;
	while(node)
	{
		if(mouse_y>=y&&mouse_y<y+24)
		{
			break;
		}
		node=node->next;
		y+=24;
	}
	if(node&&!node->st_valid&&(node->st.mode&0170000)==STAT_DIR)
	{
		strcpy(name,node->name);
	}
	file_selected=NULL;
	mutex_unlock(&files_lock);
	if(name[0])
	{
		chdir(name);
		reload_cwd();
		files_scan();
		mutex_lock(&files_lock);
		current_y=0;
		mutex_unlock(&files_lock);
	}
	paint=1;
}
void right_click_event(int mouse_x,int mouse_y)
{
	if(mouse_y>=PATH_H)
	{
		file_click(mouse_x,mouse_y);
		return;
	}
}
void event_handler(int type,int code,int x,int y)
{
	int sy,sy1,m_x,m_y;
	static int scroll_pressed,scroll_y,scroll_y1;
	static int ctrl_down;
	if(type==WMEV_DEACTIVATE)
	{
		scroll_pressed=0;
		ctrl_down=0;
	}
	else if(type==GUIEV_LCLICK)
	{
		click_event(code&0xffff,code>>16);
	}
	else if(type==GUIEV_RCLICK)
	{
		right_click_event(code&0xffff,code>>16);
	}
	else if(type==WMEV_MOUSE_Z)
	{
		if(code>0)
		{
			files_scrollup(code*24);
		}
		else
		{
			files_scrolldown(-code*24);
		}
	}
	else if(type==WMEV_KEYDOWN)
	{
		if(code==103)
		{
			files_scrollup(24);
		}
		else if(code==108)
		{
			files_scrolldown(24);
		}
		else if(code==29) // LCTRL
		{
			ctrl_down|=1;
		}
		else if(code==97) // RCTRL
		{
			ctrl_down|=2;
		}
		else if(code==19) // R
		{
			if(ctrl_down)
			{
				mutex_lock(&input_lock);
				input_x=0;
				input_y=0;
				input_buf[0]=0;
				input_valid=1;
				input_type=INPUT_CMD;
				mutex_unlock(&input_lock);
				set_dialog_mode(DIALOG_INPUT,(WINW-320)/2,(WINH-320)/2);
			}
		}
	}
	else if(type==WMEV_KEYUP)
	{
		if(code==29) // LCTRL
		{
			ctrl_down&=2;
		}
		else if(code==97) // RCTRL
		{
			ctrl_down&=1;
		}
	}
	else if(type==GUIEV_LDOWN)
	{
		mutex_lock(&files_lock);
		if(max_y)
		{
			sy=current_y;
			sy=PATH_H+1+sy*(WINH-2-PATH_H-22)/max_y;
			m_x=(short)code;
			m_y=code>>16;
			if(m_x>=WINW-10&&m_x<WINW-2&&m_y>=sy&&m_y<sy+20)
			{
				scroll_pressed=1;
				scroll_y=y-sy;
				scroll_y1=sy;
			}
		}
		mutex_unlock(&files_lock);
	}
	else if(type==GUIEV_LUP)
	{
		scroll_pressed=0;
	}
	else if(type==WMEV_CURSOR_Y)
	{
		mutex_lock(&files_lock);
		sy1=max_y;
		mutex_unlock(&files_lock);
		if(scroll_pressed&&sy1)
		{
			sy=code-y-scroll_y-scroll_y1;
			if(sy<0)
			{
				files_scrollup(-sy*sy1/(WINH-2-PATH_H-10));
			}
			else
			{
				files_scrolldown(sy*sy1/(WINH-2-PATH_H-10));
			}
			scroll_y1=code-y-scroll_y;
		}
	}
}
