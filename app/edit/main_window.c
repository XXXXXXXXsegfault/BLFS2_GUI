int paint;
unsigned int file_lock;
int mode;
int if_selected(unsigned long off)
{
	unsigned long off1,off2;
	off1=select_pos.off;
	off2=current_pos.off;
	if(off1>off2)
	{
		if(off>=off2&&off<=off1)
		{
			return 1;
		}
	}
	else
	{
		if(off>=off1&&off<=off2)
		{
			return 1;
		}
	}
	return 0;
}
void p_text(void)
{
	struct file_pos pos;
	int x,y,x1,y1;
	int c;
	mutex_lock(&file_lock);
	memcpy(&pos,&view_pos,sizeof(pos));
	y=0;
	x=0;
	while(y<CH)
	{
		c=file_getc(&pos);
		if(c==-1)
		{
			break;
		}
		x1=(x-current_x)*8+1;
		y1=y*16+24;
		if(mode==2&&if_selected(pos.off))
		{
			rect(pbuf,WINW,WINH,x1,y1,8,16,0x0060ff);
		}
		if(pos.off==current_pos.off&&!current_pos_end)
		{
			rect(pbuf,WINW,WINH,x1,y1,8,14,0xffa000);
		}
		if(c=='\t')
		{
			if(x>=current_x&&x<current_x+CW)
			{
				rect(pbuf,WINW,WINH,x1,y1,7,16,0xc0c0c0);
			}
			++x;
		}
		else if(c=='\n')
		{
			x=0;
			++y;
		}
		else if(c<32||c>126)
		{
			if(x>=current_x&&x<current_x+CW)
			{
				p_char('\?',x1,y1,0x40ff,pbuf,WINW,WINH);
			}
			++x;
		}
		else
		{
			if(x>=current_x&&x<current_x+CW)
			{
				p_char(c,x1,y1,0x0,pbuf,WINW,WINH);
			}
			++x;
		}
		if(pos.off==current_pos.off&&!current_pos_end)
		{
			rect(pbuf,WINW,WINH,x1,y1+14,8,2,0x4040);
		}
		if(!file_pos_move_right(&pos))
		{
			break;
		}
	}
	x1=(x-current_x)*8+1;
	y1=y*16+24;
	if(pos.off==current_pos.off&&current_pos_end)
	{
		rect(pbuf,WINW,WINH,x1,y1,8,14,0xffa000);
		rect(pbuf,WINW,WINH,x1,y1+14,8,2,0x4040);
	}

	rect(pbuf,WINW,WINH,0,WINH-16-1,WINW,16,0xe0e0e0);
	if(mode==1)
	{
		p_str("Insert",6,1,WINH-16-1,0x0,pbuf,WINW,WINH);
	}
	else if(mode==2)
	{
		p_str("Select",6,1,WINH-16-1,0x0,pbuf,WINW,WINH);
	}
	else if(mode==3)
	{
		p_char('>',1,WINH-16-1,0xc0,pbuf,WINW,WINH);
		p_str(cmd,strlen(cmd),1+8,WINH-16-1,0x0,pbuf,WINW,WINH);
	}
	mutex_unlock(&file_lock);
}
void paint_all(void)
{
	rect(pbuf,WINW,WINH,0,0,WINW,WINH,0xffffff);
	p_text();

	rect(pbuf,WINW,WINH,0,0,WINW,24,0xc0c0);
	rect(pbuf,WINW,WINH,WINW-40,0,40,24,0xff0000);
	rect(pbuf,WINW,WINH,WINW-40-40,0,40,24,0xff00);
	p_str("Exit",4,WINW-40+4,4,0xffffff,pbuf,WINW,WINH);
	p_str("Help",4,WINW-40-40+4,4,0x0,pbuf,WINW,WINH);

	p_str("Edit",4,4,4,0x404040,pbuf,WINW,WINH);

	rect(pbuf,WINW,WINH,0,0,WINW,1,0x404040);
	rect(pbuf,WINW,WINH,0,0,1,WINH,0x404040);
	rect(pbuf,WINW,WINH,0,WINH-1,WINW,1,0x404040);
	rect(pbuf,WINW,WINH,WINW-1,0,1,WINH,0x404040);
	rect(pbuf,WINW,WINH,0,23,WINW,1,0x404040);

	wm_paint_window(winid,pbuf);
}
void event_handler(int type,int code,int x,int y)
{
	int mouse_x,mouse_y;
	if(type==GUIEV_LCLICK)
	{
		mouse_x=(short)code;
		mouse_y=code>>16;
		if(mouse_y<24)
		{
			if(mouse_x>=WINW-40)
			{
				//exit
				exit(0);
			}
			else if(mouse_x>=WINW-40-40)
			{
				//help
				system_nonblock2("/bin/edit /doc/edit.txt");
			}
		}
	}
	else if(type==WMEV_KEYDOWN)
	{
		mutex_lock(&file_lock);
		if(code==105)
		{
			cursor_left();
			current_x_refine();
			paint=1;
		}
		else if(code==106)
		{
			cursor_right();
			current_x_refine();
			paint=1;
		}
		else if(code==103)
		{
			cursor_up();
			current_x_refine();
			paint=1;
		}
		else if(code==108)
		{
			cursor_down();
			current_x_refine();
			paint=1;
		}
		else if(code==1)
		{
			mode=0;
			paint=1;
		}
		else if(code==14)
		{
			if(mode==1)
			{
				c_delete();
				paint=1;
			}
			else if(mode==3)
			{
				int l;
				l=strlen(cmd);
				if(l)
				{
					cmd[l-1]=0;
				}
				paint=1;
			}
		}
		mutex_unlock(&file_lock);
	}
	else if(type==GUIEV_CHAR)
	{
		mutex_lock(&file_lock);
		if(mode==0)
		{
			if(code=='W')
			{
				save_file();
			}
			else if(code=='I')
			{
				mode=1;
				paint=1;
			}
			else if(code=='S')
			{
				memcpy(&select_pos,&current_pos,sizeof(current_pos));
				mode=2;
				paint=1;
			}
			else if(code=='>')
			{
				cmd[0]=0;
				mode=3;
				paint=1;
			}
			else if(code=='P')
			{
				paste_clipboard();
				paint=1;
			}
			else if(code=='U')
			{
				undo();
				paint=1;
			}
			else if(code=='R')
			{
				redo();
				paint=1;
			}
			else if(code=='f')
			{
				if(search_buf[0])
				{
					search_forward(search_buf);
					paint=1;
				}
			}
			else if(code=='F')
			{
				if(search_buf[0])
				{
					search_backward(search_buf);
					paint=1;
				}
			}
		}
		else if(mode==1)
		{
			c_insert(code);
			paint=1;
		}
		else if(mode==2)
		{
			if(code=='C')
			{
				copy_to_clipboard();
				mode=0;
				paint=1;
			}
			else if(code=='D')
			{
				copy_to_clipboard();
				delete_selected();
				mode=0;
				paint=1;
			}
		}
		else if(mode==3)
		{
			if(code=='\n')
			{
				issue_command();
				mode=0;
			}
			else
			{
				int l;
				l=strlen(cmd);
				if(l<47)
				{
					cmd[l]=code;
					cmd[l+1]=0;
				}
			}
			paint=1;
		}
		mutex_unlock(&file_lock);
	}
}
