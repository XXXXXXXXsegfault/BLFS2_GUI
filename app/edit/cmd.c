
char cmd[48];
void copy_to_clipboard(void)
{
	int fd,c;
	long int len;
	struct file_pos pos;
	if(select_pos.off<current_pos.off)
	{
		memcpy(&pos,&select_pos,sizeof(pos));
		len=current_pos.off-select_pos.off+1;
	}
	else
	{
		memcpy(&pos,&current_pos,sizeof(pos));
		len=select_pos.off-current_pos.off+1;
	}
	fd=openat(tmpfd,"clipboard",66,0644);
	if(fd<0)
	{
		return;
	}
	write(fd,&len,8);
	while((c=file_getc(&pos))!=-1&&len>0)
	{
		write(fd,&c,1);
		--len;
		file_pos_move_right(&pos);
	}
	close(fd);
}
void paste_clipboard(void)
{
	int fd;
	long int len;
	char buf[256];
	int n,x;
	len=0;
	fd=openat(tmpfd,"clipboard",0,0);
	if(fd<0)
	{
		return;
	}
	read(fd,&len,8);
	while(len>0)
	{
		if(len>256)
		{
			n=256;
		}
		else
		{
			n=len;
		}
		n=read(fd,buf,n);
		x=0;
		while(x<n)
		{
			c_insert(buf[x]);
			++x;
		}
		len-=n;
	}
	close(fd);
}
void delete_selected(void)
{
	long int len,l;
	if(select_pos.off<current_pos.off)
	{
		len=current_pos.off-select_pos.off;
		cursor_right();
	}
	else
	{
		len=select_pos.off-current_pos.off;
		l=len+1;
		while(l)
		{
			cursor_right();
			--l;
		}
	}
	++len;
	while(len)
	{
		c_delete();
		--len;
	}
}
char *find_buf;
int if_str_match(char *str)
{
	struct file_pos pos;
	int c;
	memcpy(&pos,&current_pos,sizeof(pos));
	while(*str)
	{
		c=file_getc(&pos);
		if(c==-1)
		{
			return 0;
		}
		if(c!=*(unsigned char *)str)
		{
			return 0;
		}
		if(!file_pos_move_right(&pos))
		{
			return 0;
		}
		++str;
	}
	return 1;
}
void search_forward(char *str)
{
	struct file_pos old_pos,old_view_pos;
	int old_end;
	if(*str==0)
	{
		return;
	}
	memcpy(&old_pos,&current_pos,sizeof(current_pos));
	memcpy(&old_view_pos,&view_pos,sizeof(view_pos));
	old_end=current_pos_end;
	while(cursor_right())
	{
		if(if_str_match(str))
		{
			return;
		}
	}
	memcpy(&current_pos,&old_pos,sizeof(current_pos));
	memcpy(&view_pos,&old_view_pos,sizeof(view_pos));
	current_pos_end=old_end;
}
void search_backward(char *str)
{
	struct file_pos old_pos,old_view_pos;
	int old_end;
	if(*str==0)
	{
		return;
	}
	memcpy(&old_pos,&current_pos,sizeof(current_pos));
	memcpy(&old_view_pos,&view_pos,sizeof(view_pos));
	old_end=current_pos_end;
	while(cursor_left())
	{
		if(if_str_match(str))
		{
			return;
		}
	}
	memcpy(&current_pos,&old_pos,sizeof(current_pos));
	memcpy(&view_pos,&old_view_pos,sizeof(view_pos));
	current_pos_end=old_end;
}
char search_buf[64];
void issue_command(void)
{
	long int line;
	line=0;
	if(cmd[0]=='g')
	{
		if(cmd[1]>='0'&&cmd[1]<='9')
		{
			sinputi(cmd+1,&line);
			view_pos.block=file_head;
			view_pos.off=0;
			view_pos.pos=0;
			current_pos.block=file_head;
			current_pos.off=0;
			current_pos.pos=0;
			current_x=0;
			current_pos_end=0;
			while(line>1)
			{
				cursor_down();
				--line;
			}
		}
		else if(cmd[1]=='e')
		{
			current_x=0;
			current_pos_end=0;
			while(cursor_down());
		}
	}
	else if(cmd[0]=='f')
	{
		if(cmd[1])
		{
			strcpy(search_buf,cmd+1);
			search_forward(search_buf);
		}
	}
	else if(cmd[0]=='F')
	{
		if(cmd[1])
		{
			strcpy(search_buf,cmd+1);
			search_backward(search_buf);
		}
	}
}
