int wmcall_handler(struct server_args *sargs,struct wmmsg *req)
{
	int x,y;
	unsigned int size;
	struct window *win,*new_win;
	if(req->cmd==WMCMD_CREATE_WINDOW)
	{
		x=0;
		mutex_lock(&display_lock);
		while(x<MAX_WINDOWS)
		{
			if(sargs->win[x]==NULL)
			{
				break;
			}
			++x;
		}
		if(x==MAX_WINDOWS)
		{
			req->cmd=0xfffffffe;
		}
		else
		{
			if(win=create_window(req->x,req->y,req->w,req->h))
			{
				sargs->win[x]=win;
				req->cmd=0;
				req->window_id=x;
			}
			else
			{
				req->cmd=0xfffffffd;
			}
			paint=1;
		}
		mutex_unlock(&display_lock);
		return 0;
	}
	if(req->cmd==WMCMD_DELETE_WINDOW)
	{
		mutex_lock(&display_lock);
		if(req->window_id>=MAX_WINDOWS)
		{
			req->cmd=0xfffffffc;
		}
		else if((win=sargs->win[req->window_id])==NULL)
		{
			req->cmd=0xfffffffc;
		}
		else
		{
			delete_window(win);
			sargs->win[req->window_id]=NULL;
			paint=1;
			req->cmd=0;
		}
		mutex_unlock(&display_lock);
		return 0;
	}
	if(req->cmd==WMCMD_PAINT_WINDOW)
	{
		mutex_lock(&display_lock);
		if(req->window_id>=MAX_WINDOWS)
		{
			req->cmd=0xfffffffc;
		}
		else if((win=sargs->win[req->window_id])==NULL)
		{
			req->cmd=0xfffffffc;
		}
		else
		{
			unsigned int size;
			size=win->w*win->h*4;
			if(size>sargs->size)
			{
				size=sargs->size;
			}
			if(size==0)
			{
				req->cmd=0xfffffffc;
			}
			else
			{
				memcpy(win->buf,sargs->databuf,size);
				set_pz(win->x,win->y,win->w,win->h);
				win->paint=1;
				paint=1;
				req->cmd=0;
			}
		}
		mutex_unlock(&display_lock);
		return 0;
	}
	if(req->cmd==WMCMD_GET_SCREEN_SIZE)
	{
		req->x=0;
		req->y=0;
		req->w=screen_w;
		req->h=screen_h;
		req->cmd=0;
		return 0;
	}
	if(req->cmd==WMCMD_GET_WINDOW_SIZE)
	{
		mutex_lock(&display_lock);
		if(req->window_id>=MAX_WINDOWS)
		{
			req->cmd=0xfffffffc;
		}
		else if((win=sargs->win[req->window_id])==NULL)
		{
			req->cmd=0xfffffffc;
		}
		else
		{
			req->x=win->x;
			req->y=win->y;
			req->w=win->w;
			req->h=win->h;
			req->cmd=0;
		}
		mutex_unlock(&display_lock);
		return 0;
	}
	if(req->cmd==WMCMD_READ_EVENT)
	{
		mutex_lock(&display_lock);
		if(req->window_id>=MAX_WINDOWS)
		{
			req->cmd=0xfffffffc;
		}
		else if((win=sargs->win[req->window_id])==NULL)
		{
			req->cmd=0xfffffffc;
		}
		else
		{
			req->x=win->x;
			req->y=win->y;
			req->w=win->w;
			req->h=win->h;
			y=0;
			while(y<256)
			{
				x=win->ev_y;
				if(x==win->ev_x)
				{
					break;
				}
				else
				{
					req->ev[y].type=win->evbuf[x].type;
					req->ev[y].code=win->evbuf[x].code;
					win->ev_y=x+1&4095;
				}
				++y;
			}
			req->cmd=0;
			req->nev=y;
		}
		mutex_unlock(&display_lock);
		return 0;
	}
	if(req->cmd==WMCMD_HIDE_WINDOW)
	{
		mutex_lock(&display_lock);
		if(req->window_id>=MAX_WINDOWS)
		{
			req->cmd=0xfffffffc;
		}
		else if((win=sargs->win[req->window_id])==NULL)
		{
			req->cmd=0xfffffffc;
		}
		else
		{
			send_event(WMEV_DEACTIVATE,0);
			win->paint=0;
			req->cmd=0;
			set_pz(win->x,win->y,win->w,win->h);
		}
		mutex_unlock(&display_lock);
		return 0;
	}
	if(req->cmd==WMCMD_RESIZE_WINDOW)
	{
		mutex_lock(&display_lock);
		if(req->window_id>=MAX_WINDOWS)
		{
			req->cmd=0xfffffffc;
		}
		else if((win=sargs->win[req->window_id])==NULL)
		{
			req->cmd=0xfffffffc;
		}
		else
		{
			new_win=create_window(req->x,req->y,req->w,req->h);
			if(new_win==NULL)
			{
				req->cmd=0xfffffffd;
			}
			else
			{
				new_win->ev_x=win->ev_x;
				new_win->ev_y=win->ev_y;
				memcpy(new_win->evbuf,win->evbuf,sizeof(win->evbuf));
				delete_window(win);
				sargs->win[req->window_id]=new_win;
				req->cmd=0;
			}
		}
		mutex_unlock(&display_lock);
		return 0;
	}
	return 1;
}