#include "../../include/gui.c"
#include "../../include/iformat.c"
#include "../../include/stat.c"
#define CW 80
#define CH 30
#define WINW (CW*8+10)
#define WINH (CH*16+25+24)
int winid,paint;
int mode;
char input_buf[65536];
unsigned int pbuf[WINW*WINH];
int tmpfd;
int current_y;
char tmp_path[64];

int init_tmp(void)
{
	char buf[64];
	strcpy(buf,"/tmp/UID");
	sprinti(buf,getuid(),1);
	mkdir(buf,0700);
	strcat(buf,"/browser");
	sprinti(buf,getpid(),1);
	mkdir(buf,0755);
	tmpfd=open(buf,02000000,0);
	if(tmpfd<0)
	{
		return 1;
	}
	strcpy(tmp_path,buf);
	return 0;
}
#include "pages.c"
int T_change_path(void *arg)
{
	int x;
	struct page page;
	static char buf[65536];
	mutex_lock(&page_lock);
	strcpy(buf,input_buf);
	if(!(page_load(buf,&page)&1))
	{
		if(!format_page(&page))
		{
			if(current_page==MAX_PAGES-1)
			{
				page_release(pages);
				memmove(pages,pages+1,(MAX_PAGES-1)*sizeof(struct page));
			}
			else
			{
				++current_page;
			}
			x=current_page;
			while(x<MAX_PAGES)
			{
				page_release(pages+x);
				++x;
			}
			memcpy(pages+current_page,&page,sizeof(page));
			num_pages=current_page+1;
		}
	}
	current_y=0;
	mutex_unlock(&page_lock);
	mode=0;
	paint=1;
	return 0;
}
void paint_all(void)
{
	rect(pbuf,WINW,WINH,0,0,WINW,WINH,0xffffff);
	display_page();
	rect(pbuf,WINW,WINH,0,0,WINW,24,0xc0c0);
	rect(pbuf,WINW,WINH,WINW-40,0,40,24,0xff0000);
	p_str("Web Browser",11,4,4,0x404040,pbuf,WINW,WINH);
	p_str("Exit",4,WINW-40+4,4,0xffffff,pbuf,WINW,WINH);
	rect(pbuf,WINW,WINH,0,0,WINW,1,0x404040);
	rect(pbuf,WINW,WINH,0,0,1,WINH,0x404040);
	rect(pbuf,WINW,WINH,0,WINH-1,WINW,1,0x404040);
	rect(pbuf,WINW,WINH,WINW-1,0,1,WINH,0x404040);
	rect(pbuf,WINW,WINH,0,23,WINW,1,0x404040);
	wm_paint_window(winid,pbuf);
}
void hex_append(char *buf,char c)
{
	char buf2[4];
	char *str;
	if(c>='0'&&c<='9'||c>='A'&&c<='Z'||c>='a'&&c<='z')
	{
		buf2[0]=c;
		buf2[1]=0;
	}
	else
	{
		str="0123456789ABCDEF";
		buf2[0]='%';
		buf2[1]=str[c>>4&0xf];
		buf2[2]=str[c&0xf];
		buf2[3]=0;
	}
	strcat(buf,buf2);
}
void scroll_down(int n)
{
	current_y+=n;
	if(current_y+WINH-48>page_height)
	{
		current_y=page_height+48-WINH;
	}
	if(current_y<0)
	{
		current_y=0;
	}
}
void event_handler(int type,int code,int x,int y)
{
	int m_x,m_y;
	if(type==WMEV_DEACTIVATE)
	{
		return;
	}
	if(type==WMEV_MOUSE_Z)
	{
		scroll_down(-code*16);
		paint=1;
		return;
	}
	if(type==GUIEV_LCLICK)
	{
		m_x=(short)code;
		m_y=code>>16;
		if(m_y<24)
		{
			if(m_x>=WINW-40)
			{
				exit(0);
			}
		}
		else if(m_y<48)
		{
			if(mode==0)
			{
				input_buf[0]=0;
				mode=1;
				paint=1;
			}
		}
		else if(!mode)
		{
			struct page_element *pe,*node,*fbeg;
			int x1;
			mutex_lock(&page_lock);
			pe=get_page_element_by_pos(m_x,m_y);
			if(pe)
			{
				if(pe->type==E_LINK)
				{
					strcpy(input_buf,pages[current_page].pagepath);
					if(!strncmp(pe->str,"http://",7)||!strncmp(pe->str,"https://",8))
					{
						strcpy(input_buf,pe->str);
					}
					else if(pe->str[0]=='/')
					{
						x1=7;
						if(!strncmp(input_buf,"https://",8))
						{
							x1=8;
						}
						while(input_buf[x1]&&input_buf[x1]!='/')
						{
							++x1;
						}
						strcpy(input_buf+x1,pe->str);
					}
					else
					{
						x1=strlen(input_buf);
						while(x1)
						{
							if(input_buf[x1]=='/')
							{
								if(x1<=8)
								{
									strcat(input_buf,"/");
								}
								else
								{
									input_buf[x1+1]=0;
								}
								break;
							}
							--x1;
						}
						strcat(input_buf,pe->str);
					}
					create_lwp(1048576,T_change_path,0);
				}
				else if(pe->type==E_INBX)
				{
					current_input=pe;
					paint=1;
				}
				else if(pe->type==E_SUBM)
				{
					fbeg=NULL;
					node=page_elements;
					while(node)
					{
						if(node==pe)
						{
							break;
						}
						if(node->type==E_FBEG)
						{
							fbeg=node;
						}
						node=node->next;
					}
					if(fbeg)
					{
						strcpy(input_buf,pages[current_page].pagepath);
						x1=0;
						while(fbeg->type!=E_FEND)
						{
							if(fbeg->type==E_ACTN)
							{
								int l;
								if(fbeg->str[0]=='/')
								{
									l=8;
									while(input_buf[l]&&input_buf[l]!='/')
									{
										++l;
									}
									strcpy(input_buf+l,fbeg->str);
								}
								else
								{
									l=strlen(input_buf);
									while(l>8&&input_buf[l]!='/')
									{
										--l;
									}
									if(l==8)
									{
										strcat(input_buf,"/");
									}
									else
									{
										input_buf[l+1]=0;
									}
									strcat(input_buf,fbeg->str);
								}
							}
							else if(fbeg->type==E_ARGM)
							{
								if(x1)
								{
									strcat(input_buf,"&");
								}
								else
								{
									strcat(input_buf,"\?");
								}
								strcat(input_buf,fbeg->str);
								strcat(input_buf,"=");
								strcat(input_buf,fbeg->str2);
								++x1;
							}
							else if(fbeg->type==E_INBX)
							{
								if(x1)
								{
									strcat(input_buf,"&");
								}
								else
								{
									strcat(input_buf,"\?");
								}
								strcat(input_buf,fbeg->str);
								strcat(input_buf,"=");
								if(fbeg->buf)
								{
									int x2;
									x2=0;
									while(fbeg->buf[x2])
									{
										hex_append(input_buf,fbeg->buf[x2]);
										++x2;
									}
								}
								++x1;
							}
							fbeg=fbeg->next;
						}
						create_lwp(1048576,T_change_path,0);
					}
				}
			}
			mutex_unlock(&page_lock);
		}
		return;
	}
	if(type==WMEV_KEYDOWN)
	{
		if(mode==1)
		{
			if(code==14)
			{
				int l;
				l=strlen(input_buf);
				if(l)
				{
					input_buf[l-1]=0;
					paint=1;
				}
			}
		}
		else
		{
			if(code==14)
			{
				mutex_lock(&page_lock);
				if(current_input)
				{
					int l;
					l=strlen(current_input->buf);
					if(l)
					{
						current_input->buf[l-1]=0;
						paint=1;
					}
				}
				mutex_unlock(&page_lock);
			}
			if(code==103)
			{
				scroll_down(-16);
				paint=1;
			}
			if(code==108)
			{
				scroll_down(16);
				paint=1;
			}
			if(code==105)
			{
				mutex_lock(&page_lock);
				if(current_page)
				{
					--current_page;
					current_y=0;
					format_page(pages+current_page);
				}
				mutex_unlock(&page_lock);
				paint=1;
			}
			if(code==106)
			{
				mutex_lock(&page_lock);
				if(current_page<num_pages-1)
				{
					++current_page;
					current_y=0;
					format_page(pages+current_page);
				}
				mutex_unlock(&page_lock);
				paint=1;
			}
		}
		return;
	}
	if(type==GUIEV_CHAR)
	{
		if(mode==1)
		{
			if(code>=32&&code<=126)
			{
				int l;
				l=strlen(input_buf);
				if(l!=4095)
				{
					input_buf[l]=code;
					input_buf[l+1]=0;
					paint=1;
				}
			}
			else if(code=='\n')
			{
				create_lwp(1048576,T_change_path,0);
			}
		}
		else if(mode==0)
		{
			mutex_lock(&page_lock);
			if(current_input)
			{
				if(code>=32&&code<=126)
				{
					int l;
					l=strlen(current_input->buf);
					if(l!=4095)
					{
						current_input->buf[l]=code;
						current_input->buf[l+1]=0;
						paint=1;
					}
				}
				else if(code=='\n')
				{
					current_input=NULL;
					paint=1;
				}
			}
			mutex_unlock(&page_lock);
		}
	}
}
int main(void)
{
	if(init_tmp())
	{
		return 1;
	}
	num_pages=1;
	if(page_load("file://doc/browser_welcome.html",pages))
	//if(page_load("file://home/test/xxx.html",pages))
	{
		return 1;
	}
	if(format_page(pages))
	{
		return 1;
	}
	if(wm_connect())
	{
		return 1;
	}
	wm_get_screen_size();
	winid=wm_create_window((screen_size[0]-WINW)/2,(screen_size[1]-WINH)/2,WINW,WINH);
	if(winid<0)
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
