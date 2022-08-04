struct window
{
	int x;
	int y;
	int w;
	int h;
	struct window *next;
	struct window *prev;
	unsigned int *buf;
	struct wmev evbuf[4096];
	int ev_x;
	int ev_y;
	long int paint;
} *win_start,*win_end;
struct window *win_moving;
unsigned int display_lock;
unsigned int *bg_buf;
unsigned int screen_w,screen_h,screen_refresh;
int cursor_x,cursor_y;
unsigned int def_cursor[441];
unsigned int paint;
void send_event(int type,int code)
{
	struct window *win;
	int x;
	if(win_moving)
	{
		return;
	}
	win=win_end;
	while(win&&win->paint==0)
	{
		win=win->prev;
	}
	if(!win)
	{
		return;
	}
	x=win->ev_x;
	if((x+1&4095)==win->ev_y)
	{
		return;
	}
	win->evbuf[x].type=type;
	win->evbuf[x].code=code;
	win->ev_x=x+1&4095;
}
unsigned long bbcall(struct bbreq *req,void *data,unsigned int datasize)
{
	unsigned long ret;
	if(datasize)
	{
		memcpy(shm,data,datasize);
	}
	write(bbfd,req,sizeof(*req));
	ret=0xffffffffffffffff;
	sock_read(bbfd,&ret,8);
	return ret;
}
unsigned int get_bg_color(int x,int y)
{
	long int X,Y;
	int i;
	int x1,y1;
	int x2,y2;
	unsigned int c[4];
	unsigned int r[4];
	unsigned int g[4];
	unsigned int b[4];
	unsigned long int R[2],G[2],B[2];
	X=(long int)x*bg_size[0];
	Y=(long int)y*bg_size[1];
	x1=X/screen_w;
	y1=Y/screen_h;
	x2=X%screen_w;
	y2=Y%screen_h;
	memset(c,0,16);
	c[0]=bg_map[y1*bg_size[0]+x1];
	if(x1<bg_size[0]-1)
	{
		c[1]=bg_map[y1*bg_size[0]+x1+1];
		if(y1<bg_size[1]-1)
		{
			c[3]=bg_map[(y1+1)*bg_size[0]+x1+1];
		}
	}
	if(y1<bg_size[1]-1)
	{
		c[2]=bg_map[(y1+1)*bg_size[0]+x1];
	}
	i=0;
	while(i<4)
	{
		r[i]=c[i]>>16&0xff;
		g[i]=c[i]>>8&0xff;
		b[i]=c[i]&0xff;
		++i;
	}
	R[0]=r[1]*x2+r[0]*(screen_w-x2-1);
	G[0]=g[1]*x2+g[0]*(screen_w-x2-1);
	B[0]=b[1]*x2+b[0]*(screen_w-x2-1);
	R[1]=r[3]*x2+r[2]*(screen_w-x2-1);
	G[1]=g[3]*x2+g[2]*(screen_w-x2-1);
	B[1]=b[3]*x2+b[2]*(screen_w-x2-1);
	R[0]=R[1]*y2+R[0]*(screen_h-y2-1);
	G[0]=G[1]*y2+G[0]*(screen_h-y2-1);
	B[0]=B[1]*y2+B[0]*(screen_h-y2-1);
	R[0]/=(screen_w-1)*(screen_h-1);
	G[0]/=(screen_w-1)*(screen_h-1);
	B[0]/=(screen_w-1)*(screen_h-1);
	return R[0]<<16|G[0]<<8|B[0];
}
void display_bg(void)
{
	struct bbreq req;
	unsigned long ret;
	int x,y;
	unsigned int *ptr;
	if(bg_buf==NULL)
	{
		if(bg_buf=malloc(screen_w*screen_h*4))
		{
			if(bg_map)
			{
				ptr=bg_buf;
				y=0;
				while(y<screen_h)
				{
					x=0;
					while(x<screen_w)
					{
						*ptr=get_bg_color(x,y);
						++ptr;
						++x;
					}
					++y;
				}
			}
			else
			{
				memset(bg_buf,0,screen_w*screen_h*4);
			}
		}
	}
	if(bg_buf==NULL)
	{
		memset(&req,0,sizeof(req));
		req.cmd=BBCMD_CLEAR;
		bbcall(&req,NULL,0);
	}
	else
	{
		memset(&req,0,sizeof(req));
		req.cmd=BBCMD_BLIT_XRGB;
		req.w=screen_w;
		req.h=screen_h;
		bbcall(&req,bg_buf,screen_w*screen_h*4);
	}
}
struct pz
{
	int x;
	int y;
	int w;
	int h;
} pz;
void _set_pz(struct pz *pz,int x,int y,int w,int h)
{
	int x1,y1,x2,y2;
	if(x<0)
	{
		w+=x;
		x=0;
	}
	if(y<0)
	{
		h+=y;
		y=0;
	}
	if(x+w>screen_w)
	{
		w=screen_w-x;
	}
	if(y+h>screen_h)
	{
		h=screen_h-y;
	}
	if(w<=0||h<=0)
	{
		return;
	}
	if(pz->w<=0||pz->h<=0)
	{
		pz->x=x;
		pz->y=y;
		pz->w=w;
		pz->h=h;
		return;
	}
	x1=pz->x+pz->w;
	y1=pz->y+pz->h;
	x2=x+w;
	y2=y+h;
	if(x<pz->x)
	{
		pz->x=x;
	}
	if(y<pz->y)
	{
		pz->y=y;
	}
	if(x2>x1)
	{
		x1=x2;
	}
	if(y2>y1)
	{
		y1=y2;
	}
	pz->w=x1-pz->x;
	pz->h=y1-pz->y;
}
void set_pz(int x,int y,int w,int h)
{
	_set_pz(&pz,x,y,w,h);
}
void display_all_nolock(void)
{
	struct bbreq req;
	struct window *win;
	req.cmd=BBCMD_SET_PZ;
	req.x=pz.x;
	req.y=pz.y;
	req.w=pz.w;
	req.h=pz.h;
	bbcall(&req,NULL,0);
	pz.w=0;
	

	display_bg();
	win=win_start;
	req.cmd=BBCMD_BLIT_XRGB;
	while(win)
	{
		if(win->paint)
		{
			req.x=win->x;
			req.y=win->y;
			req.w=win->w;
			req.h=win->h;
			bbcall(&req,win->buf,win->w*win->h*4);
		}
		win=win->next;
	}
	req.cmd=BBCMD_BLIT_MRGB;
	req.x=cursor_x-10;
	req.y=cursor_y-10;
	req.w=21;
	req.h=21;
	bbcall(&req,def_cursor,441*4);
}
void display_buf_flush(void)
{
	struct bbreq req;
	memset(&req,0,sizeof(req));
	req.cmd=BBCMD_DISPLAY;
	bbcall(&req,NULL,0);
}
struct window *create_window(int x,int y,int w,int h)
{
	struct window *win;
	unsigned int *buf;
	if(w<=0||h<=0||x+w<=0||y+h<=0||x>=screen_w||y>=screen_h||w>screen_w||h>screen_h)
	{
		return NULL;
	}
	win=malloc(sizeof(*win));
	if(win==NULL)
	{
		return NULL;
	}
	buf=mmap(0,w*h*4,3,0x22,-1,0);
	if(!valid(buf))
	{
		free(win);
		return NULL;
	}
	win->buf=buf;
	win->x=x;
	win->y=y;
	win->w=w;
	win->h=h;
	win->ev_x=0;
	win->ev_y=0;
	send_event(WMEV_DEACTIVATE,0);
	win->next=NULL;
	win->prev=win_end;
	win->paint=0;
	if(win_end)
	{
		win_end->next=win;
	}
	else
	{
		win_start=win;
	}
	win_end=win;
	set_pz(x,y,w,h);
	return win;
}
void delete_window(struct window *win)
{
	struct window *prev,*next;
	if(!win)
	{
		return;
	}
	prev=win->prev;
	next=win->next;
	set_pz(win->x,win->y,win->w,win->h);
	set_pz(cursor_x-10,cursor_y-10,21,21);
	if(prev)
	{
		prev->next=next;
	}
	else
	{
		win_start=next;
	}
	if(next)
	{
		next->prev=prev;
	}
	else
	{
		win_end=prev;
	}
	if(win_moving==win)
	{
		win_moving=NULL;
	}
	munmap(win->buf,win->w*win->h*4);
	free(win);
}
void activate_window(struct window *win)
{
	struct window *prev,*next;
	if(!win)
	{
		return;
	}
	prev=win->prev;
	next=win->next;
	set_pz(win->x,win->y,win->w,win->h);
	set_pz(cursor_x-10,cursor_y-10,21,21);
	if(prev)
	{
		prev->next=next;
	}
	else
	{
		win_start=next;
	}
	if(next)
	{
		next->prev=prev;
	}
	else
	{
		win_end=prev;
	}
	win->prev=win_end;
	win->next=NULL;
	if(win_end)
	{
		win_end->next=win;
	}
	else
	{
		win_start=win;
	}
	win_end=win;
}
void display_init(void)
{
	struct bbreq req;
	unsigned long ret;
	memset(&req,0,sizeof(req));
	req.cmd=BBCMD_GET_SIZE;
	ret=bbcall(&req,NULL,0);
	screen_w=ret&0xffff;
	screen_h=ret>>16&0xffff;
	shmid=shmget('W',screen_w*screen_h*4+4096,0600);
	shm=shmat(shmid,NULL,0);
	if(!valid(shm))
	{
		exit(1);
	}
	screen_refresh=ret>>32&0xffff;
	cursor_x=screen_w/2;
	cursor_y=screen_h/2;
	memset(def_cursor,0xff,441*4);
	rect(def_cursor,21,21,0,9,21,3,0xffffff);
	rect(def_cursor,21,21,9,0,3,21,0xffffff);
	rect(def_cursor,21,21,1,10,19,1,0);
	rect(def_cursor,21,21,10,1,1,19,0);

	set_pz(0,0,screen_w,screen_h);
	display_all_nolock();
	display_buf_flush();
}
