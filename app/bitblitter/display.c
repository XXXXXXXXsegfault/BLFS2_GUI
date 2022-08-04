#define NFBS 4
struct display_fb
{
	unsigned int *buf;
	unsigned int fb_id;
	unsigned int handle;
	unsigned long long int size;
	unsigned short width;
	unsigned short height;
	unsigned int line_length;
} fb[NFBS];
int def_w,def_h;
struct drm_mode_card_res *cres;
unsigned int conn_id;
struct drm_mode_get_connector *conn;
struct drm_mode_modeinfo *mode;
int display_fd;
unsigned int *display_buf;
int current_fb;
struct pz
{
	int x;
	int y;
	int w;
	int h;
} pz,fbpz[NFBS];
void set_pz(struct pz *pz,int x,int y,int w,int h)
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
	if(x+w>mode->hdisplay)
	{
		w=mode->hdisplay-x;
	}
	if(y+h>mode->vdisplay)
	{
		h=mode->vdisplay-y;
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
int drm_create_fb(int fd,int width,int height,struct display_fb *fb)
{
	struct drm_mode_create_dumb cdumb;
	struct drm_mode_fb_cmd2 fbcmd;
	struct drm_mode_map_dumb mdumb;
	unsigned int *sptr;
	memset(&cdumb,0,sizeof(cdumb));
	memset(&fbcmd,0,sizeof(fbcmd));
	memset(&mdumb,0,sizeof(mdumb));
	cdumb.height=height;
	cdumb.width=width;
	cdumb.bpp=32;
	if(ioctl(fd,DRM_IOCTL_MODE_CREATE_DUMB,&cdumb))
	{
		return 1;
	}
	fbcmd.width=cdumb.width;
	fbcmd.height=cdumb.height;
	fbcmd.pixel_format=0x34325258;
	fbcmd.pitches[0]=cdumb.pitch;
	fbcmd.handles[0]=cdumb.handle;
	if(ioctl(fd,DRM_IOCTL_MODE_ADDFB2,&fbcmd))
	{
		ioctl(fd,DRM_IOCTL_MODE_DESTROY_DUMB,&cdumb.handle);
		return 1;
	}
	mdumb.handle=cdumb.handle;
	if(ioctl(fd,DRM_IOCTL_MODE_MAP_DUMB,&mdumb))
	{
		ioctl(fd,DRM_IOCTL_MODE_RMFB,&fbcmd.fb_id);
		ioctl(fd,DRM_IOCTL_MODE_DESTROY_DUMB,&cdumb.handle);
		return 1;
	}
	if(!valid(sptr=mmap(0,cdumb.size,3,1,fd,mdumb.offset)))
	{
		ioctl(fd,DRM_IOCTL_MODE_RMFB,&fbcmd.fb_id);
		ioctl(fd,DRM_IOCTL_MODE_DESTROY_DUMB,&cdumb.handle);
		return 1;
	}
	memset(sptr,0,cdumb.size);
	fb->buf=sptr;
	fb->fb_id=fbcmd.fb_id;
	fb->handle=cdumb.handle;
	fb->size=cdumb.size;
	fb->line_length=cdumb.pitch;
	fb->width=width;
	fb->height=height;
	return 0;
}
void drm_destroy_fb(int fd,struct display_fb *fb)
{
	munmap(fb->buf,fb->size);
	ioctl(fd,DRM_IOCTL_MODE_RMFB,&fb->fb_id);
	ioctl(fd,DRM_IOCTL_MODE_DESTROY_DUMB,&fb->handle);
}
int drm_best_mode(void)
{
	int w,h,r,ret;
	int x;
	w=200;
	h=200;
	r=0;
	ret=-1;
	x=conn->count_modes;
	while(x)
	{
		--x;
		if(def_w&&conn->modes_ptr[x].hdisplay==def_w&&
		conn->modes_ptr[x].vdisplay==def_h&&
		conn->modes_ptr[x].vrefresh>=r||
		!def_w&&(long)conn->modes_ptr[x].hdisplay*
		(long)conn->modes_ptr[x].vdisplay*
		(long)conn->modes_ptr[x].vrefresh>
		(long)w*(long)h*(long)r)
		{
			w=conn->modes_ptr[x].hdisplay;
			h=conn->modes_ptr[x].vdisplay;
			r=conn->modes_ptr[x].vrefresh;
			mode=conn->modes_ptr+x;
			ret=0;
		}
	}
	return ret;
}
int display_probe(int fd)
{
	struct drm_mode_crtc crtc;
	int x;
	x=0;
	memset(&crtc,0,sizeof(crtc));
	if((cres=drm_mode_get_res_info(fd))==0)
	{
		return -1;
	}
	if(cres->count_crtcs==0||cres->count_connectors==0)
	{
		drm_mode_release_res_info(cres);
		return -1;
	}
	while(x<cres->count_connectors)
	{
		if((conn=drm_mode_get_connector_info(fd,cres->connector_id_ptr[x]))!=0)
		{
			if(!drm_best_mode())
			{
				conn_id=cres->connector_id_ptr[x];
				break;
			}
			drm_mode_release_connector_info(conn);
		}
		++x;
	}
	if(x==cres->count_connectors)
	{
		drm_mode_release_res_info(cres);
		return -1;
	}
	x=0;
	while(x<NFBS)
	{
		if(drm_create_fb(fd,mode->hdisplay,mode->vdisplay,fb+x))
		{
			while(x)
			{
				--x;
				drm_destroy_fb(fd,fb+x);
			}
			drm_mode_release_connector_info(conn);
			drm_mode_release_res_info(cres);
			return -1;
		}
		++x;
	}
	if((display_buf=malloc(fb[0].size))==0)
	{
		x=NFBS;
		while(x)
		{
			--x;
			drm_destroy_fb(fd,fb+x);
		}
		drm_mode_release_connector_info(conn);
		drm_mode_release_res_info(cres);
		return -1;
	}
	crtc.set_connectors_ptr=&conn_id;
	crtc.count_connectors=1;
	crtc.crtc_id=cres->crtc_id_ptr[0];
	crtc.fb_id=fb[0].fb_id;
	crtc.mode_valid=1;
	memcpy(&crtc.mode,mode,sizeof(*mode));
	if(ioctl(fd,DRM_IOCTL_MODE_SETCRTC,&crtc))
	{
		free(display_buf);
		x=NFBS;
		while(x)
		{
			--x;
			drm_destroy_fb(fd,fb+x);
		}
		drm_mode_release_connector_info(conn);
		drm_mode_release_res_info(cres);
		return -1;
	}
	x=0;
	while(x<NFBS)
	{
		set_pz(fbpz+x,0,0,mode->hdisplay,mode->vdisplay);
		++x;
	}
	return 0;
}
int display_init(void)
{
	int fd;
	int x;
	char buf[32];
	static unsigned int buf2[128];
	// get default resolution
	fd=open("/dev/fb0",0,0);
	if(fd>=0)
	{
		ioctl(fd,0x4600,buf2);
		def_w=buf2[0];
		def_h=buf2[1];
		close(fd);
	}
	x=0;
	while(x<16)
	{
		strcpy(buf,"/dev/dri/card");
		sprinti(buf,x,1);
		fd=open(buf,02000002,0);
		if(fd>=0)
		{
			if(!display_probe(fd))
			{
				display_fd=fd;
				return 0;
			}
			close(fd);
		}
		++x;
	}
	return -1;
}
void display_buf_display(void)
{
	struct drm_mode_crtc_page_flip pf;
	int off,x,y,w,h,fbl;
	int i;
	struct pz new_pz;
	static int s;
	current_fb=(current_fb+1)%NFBS;
	memset(&pf,0,sizeof(pf));
	i=0;
	memset(&new_pz,0,sizeof(new_pz));
	while(i<NFBS)
	{
		x=fbpz[i].x;
		y=fbpz[i].y;
		w=fbpz[i].w;
		h=fbpz[i].h;
		set_pz(&new_pz,x,y,w,h);
		++i;
	}
	x=new_pz.x;
	y=new_pz.y;
	w=new_pz.w;
	h=new_pz.h;
	fbl=fb[0].line_length/4;
	if(w>0&&h>0)
	{
		off=y*fbl+x;
		while(h)
		{
			_memcpy(fb[current_fb].buf+off,display_buf+off,w*4);
			off+=fbl;
			--h;
		}
	}
	fbpz[current_fb].w=0;
	pf.crtc_id=cres->crtc_id_ptr[0];
	pf.fb_id=fb[current_fb].fb_id;
	while(ioctl(display_fd,DRM_IOCTL_MODE_PAGE_FLIP,&pf))
	{
		sleep(0,50000/mode->vrefresh);
	}
}
