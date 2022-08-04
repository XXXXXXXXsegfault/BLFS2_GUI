#include "../../include/bitblitter.c"
void blend(unsigned int *dst,unsigned int *src,unsigned int n)
{
	static unsigned int mask[256];
	unsigned int c1,c2,m;
	mask[0]=0xffffff;
	while(n)
	{
		c1=*dst;
		c2=*src;
		m=mask[c2>>24];
		c2&=m;
		c1&=~m;
		*dst=c1|c2;
		++dst;
		++src;
		--n;
	}
}
void handle_req(int cfd)
{
	struct bbreq req;
	unsigned int *buf,*screen,*fbptr;
	int n;
	unsigned long ret;
	int fbl,image_w;
	fbl=fb[0].line_length/4;
	while(1)
	{
		if(sock_read(cfd,&req,sizeof(req))!=sizeof(req))
		{
			return;
		}
		if(req.cmd==BBCMD_GET_SIZE)
		{
			ret=mode->vrefresh;
			ret=ret<<16|mode->vdisplay;
			ret=ret<<16|mode->hdisplay;
			write(cfd,&ret,8);
		}
		else if(req.cmd==BBCMD_BLIT_XRGB||req.cmd==BBCMD_BLIT_MRGB)
		{
			if(pz.w>0&&pz.h>0)
			{
				buf=shm;
				fbptr=buf;
				image_w=req.w;
				if(req.x<pz.x)
				{
					fbptr+=pz.x-req.x;
					req.w+=req.x-pz.x;
					req.x=pz.x;
				}
				if(req.y<pz.y)
				{
					fbptr+=(pz.y-req.y)*image_w;
					req.h+=req.y-pz.y;
					req.y=pz.y;
				}
				if(req.x+req.w>pz.x+pz.w)
				{
					req.w=pz.x+pz.w-req.x;
				}
				if(req.y+req.h>pz.y+pz.h)
				{
					req.h=pz.y+pz.h-req.y;
				}
				if(req.w>0&&req.h>0)
				{
					screen=display_buf+fbl*req.y+req.x;
					if(req.cmd==BBCMD_BLIT_XRGB)
					{
						while(req.h)
						{
							_memcpy(screen,fbptr,(unsigned int)req.w*4);
							screen+=fbl;
							fbptr+=image_w;
							--req.h;
						}
					}
					else
					{
						while(req.h)
						{
							blend(screen,fbptr,req.w);
							screen+=fbl;
							fbptr+=image_w;
							--req.h;
						}
					}
				}
			}
			ret=0;
			write(cfd,&ret,8);
		}
		else if(req.cmd==BBCMD_CLEAR)
		{
			if(pz.w>0&&pz.h>0)
			{
				screen=display_buf+fbl*pz.y+pz.x;
				n=pz.h;
				while(n)
				{
					memset(screen,0,(unsigned int)pz.w*4);
					screen+=fbl;
					--n;
				}
			}
			ret=0;
			write(cfd,&ret,8);
		}
		else if(req.cmd==BBCMD_DISPLAY)
		{
			display_buf_display();
			ret=0;
			write(cfd,&ret,8);
			pz.w=0;
		}
		else if(req.cmd==BBCMD_SET_PZ)
		{
			set_pz(&pz,req.x,req.y,req.w,req.h);
			set_pz(fbpz+current_fb,req.x,req.y,req.w,req.h);
			ret=0;
			write(cfd,&ret,8);
		}
		else
		{
			ret=0xffffffffffffffff;
			write(cfd,&ret,8);
		}
	}
}
