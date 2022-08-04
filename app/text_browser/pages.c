#define MAX_PAGES 96
unsigned int page_lock;
struct page
{
	char *pagepath;
	char pagename[40];
} pages[MAX_PAGES];
int num_pages,current_page;
char *page_map;
unsigned long page_map_size;
int page_load_file(char *path,struct page *page)
{
	unsigned char name[40];
	static char buf[4096];
	int fd,fdo;
	int x;
	struct stat st;
	getrandom(name,39,1);
	name[39]=0;
	x=0;
	while(x<39)
	{
		name[x]=name[x]%8+'0';
		++x;
	}
	fd=open(path,0,0);
	if(fd<0)
	{
		return 1;
	}
	if(fstat(fd,&st)||(st.mode&0170000)!=STAT_REG||(fdo=openat(tmpfd,name,578|0200,0644))<0)
	{
		close(fd);
		return 1;
	}
	while((x=read(fd,buf,4096))>0)
	{
		write(fdo,buf,x);
	}
	close(fdo);
	close(fd);
	memcpy(page->pagename,name,40);
	return 0;
}
void page_release(struct page *page)
{
	if(page->pagepath==NULL)
	{
		return;
	}
	unlinkat(tmpfd,page->pagename,0);
	free(page->pagepath);
	page->pagepath=NULL;
}
#include "http_load.c"
int page_load(char *path,struct page *page)
{
	int status;
	char *buf;
	long int l;
	if(!strncmp(path,"file://",7))
	{
		status=page_load_file(path+6,page);
		if(status)
		{
			return 1;
		}
	}
	else if(!strncmp(path,"http://",7))
	{
		status=http_load("/bin/http_get",path+7,page);
		if(status&1)
		{
			return 1;
		}
		if(status&2)
		{
			return 2;
		}
	}
	else if(!strncmp(path,"https://",8))
	{
		status=http_load("/bin/https_get",path+8,page);
		if(status&1)
		{
			return 1;
		}
		if(status&2)
		{
			return 2;
		}
	}
	else
	{
		return 1;
	}
	l=strlen(path);
	buf=malloc(l+1);
	if(buf==NULL)
	{
		unlinkat(tmpfd,page->pagename,0);
		return 1;
	}
	memcpy(buf,path,l+1);
	page->pagepath=buf;
	return 0;
}
struct page_element
{
	unsigned long type;
	char *str;
	char *str2;
	char *buf; // for input boxes
	struct page_element *next;
} *page_elements;
#define E_TEXT 1
#define E_LINK 2
#define E_NEWL 3
#define E_FBEG 4
#define E_FEND 5
#define E_ARGM 6
#define E_INBX 7
#define E_SUBM 8
#define E_TITL 9
#define E_ACTN 10
struct page_element *current_input;
struct page_element *page_element_new(void)
{
	struct page_element *node;
	node=malloc(sizeof(*node));
	if(node==NULL)
	{
		exit(1);
	}
	memset(node,0,sizeof(*node));
	return node;
}
int page_height;
int get_page_height(void);
int format_page(struct page *page)
{
	char path[256],path2[256];
	int pid,status;
	struct page_element *node,*end;
	char *map,*str;
	int fd;
	long int size;
	strcpy(path,tmp_path);
	strcat(path,"/");
	strcat(path,page->pagename);
	strcpy(path2,tmp_path);
	strcat(path2,"/current_page.new");
	pid=fork();
	if(pid<0)
	{
		return 1;
	}
	if(pid==0)
	{
		char *argv[4];
		argv[0]="html_parse";
		argv[1]=path;
		argv[2]=path2;
		argv[3]=NULL;
		execv("/bin/html_parse",argv);
		exit(1);
	}
	status=0;
	waitpid(pid,&status,0);
	if(status)
	{
		return 1;
	}
	fd=openat(tmpfd,"current_page.new",0,0);
	if(fd<0)
	{
		return 1;
	}
	size=lseek(fd,0,2);
	if(size<=0)
	{
		close(fd);
		return 1;
	}
	map=mmap(0,size,1,1,fd,0);
	if(!valid(map))
	{
		close(fd);
		return 1;
	}
	munmap(page_map,page_map_size);
	unlinkat(tmpfd,"current_page",0);
	renameat(tmpfd,"current_page.new",tmpfd,"current_page");
	page_map=map;
	page_map_size=size;
	while(node=page_elements)
	{
		page_elements=node->next;
		free(node->buf);
		free(node);
	}
	if(size)
	{
		end=NULL;
		str=map;
		while((long)str-(long)map<size)
		{
			node=NULL;
			if(!strcmp(str,"TEXT"))
			{
				str+=5;
				node=page_element_new();
				node->str=str;
				node->type=E_TEXT;
				str+=strlen(str)+1;
			}
			else if(!strcmp(str,"TITL"))
			{
				str+=5;
				node=page_element_new();
				node->str=str;
				node->type=E_TITL;
				str+=strlen(str)+1;
			}
			else if(!strcmp(str,"LINK"))
			{
				str+=5;
				node=page_element_new();
				node->str=str;
				node->type=E_LINK;
				str+=strlen(str)+1;
				node->str2=str;
				str+=strlen(str)+1;
			}
			else if(!strcmp(str,"NEWL"))
			{
				str+=5;
				node=page_element_new();
				node->type=E_NEWL;
			}
			else if(!strcmp(str,"FBEG"))
			{
				str+=5;
				node=page_element_new();
				node->type=E_FBEG;
			}
			else if(!strcmp(str,"FEND"))
			{
				str+=5;
				node=page_element_new();
				node->type=E_FEND;
			}
			else if(!strcmp(str,"ARGM"))
			{
				str+=5;
				node=page_element_new();
				node->type=E_ARGM;
				node->str=str;
				str+=strlen(str)+1;
				node->str2=str;
				str+=strlen(str)+1;
			}
			else if(!strcmp(str,"INBX"))
			{
				str+=5;
				node=page_element_new();
				node->type=E_INBX;
				node->str=str;
				node->buf=malloc(4096);
				if(node->buf==NULL)
				{
					exit(1);
				}
				node->buf[0]=0;
				str+=strlen(str)+1;
			}
			else if(!strcmp(str,"SUBM"))
			{
				str+=5;
				node=page_element_new();
				node->type=E_SUBM;
				node->str=str;
				str+=strlen(str)+1;
			}
			else if(!strcmp(str,"ACTN"))
			{
				str+=5;
				node=page_element_new();
				node->type=E_ACTN;
				node->str=str;
				str+=strlen(str)+1;
			}
			else
			{
				str+=strlen(str)+1;
			}
			if(node)
			{
				if(end)
				{
					end->next=node;
				}
				else
				{
					page_elements=node;
				}
				end=node;
			}
		}
	}
	current_input=NULL;
	page_height=get_page_height();
	return 0;
}
int display_text(char *ptr,int *cx,int *x,int *y,unsigned int color,int p,int *line)
{
	char c;
	++*cx;
	*x+=8;
	if(*cx>=CW)
	{
		*cx=0;
		*x=4;
		*y+=16;
		if(*y>WINH&&p)
		{
			return 1;
		}
	}
	while(c=*ptr)
	{
		if(c>=32&&c<=126||c=='\t')
		{
			if(p)
			{
				p_char(c,*x,*y,color,pbuf,WINW,WINH);
			}
			if(c!=32&&c!='\t')
			{
				*line=1;
			}
			++*cx;
			*x+=8;
			
		}
		else if(c!='\n'&&c!='\r'&&c!='\v')
		{
			if(p)
			{
				p_char('\?',*x,*y,color,pbuf,WINW,WINH);
			}
			++*cx;
			*x+=8;
		}
		if(*cx>=CW||c=='\n')
		{
			*cx=0;
			*x=4;
			if(*line)
			{
				*y+=16;
				*line=0;
			}
			if(*y>WINH&&p)
			{
				return 1;
			}
		}
		++ptr;
	}
	return 0;
}
int display_page(void)
{
	int l;
	int x,y,cx,line;
	int ret;
	struct page_element *node;
	mutex_lock(&page_lock);
	node=page_elements;
	x=4;
	y=48-current_y;
	cx=0;
	ret=0;
	line=0;
	while(node)
	{
		if(node->type==E_TEXT)
		{
			if(display_text(node->str,&cx,&x,&y,0x0,1,&line))
			{
				ret=1;
				break;
			}
		}
		else if(node->type==E_LINK)
		{
			if(display_text(node->str2,&cx,&x,&y,0x60d0,1,&line))
			{
				ret=1;
				break;
			}
		}
		else if(node->type==E_NEWL)
		{
			cx=0;
			x=4;
			y+=16;
		}
		else if(node->type==E_INBX)
		{
			x=4;
			y+=16;
			cx=0;
			if(node==current_input)
			{
				rect(pbuf,WINW,WINH,x,y+15,240,1,0xb000b0);
			}
			else
			{
				rect(pbuf,WINW,WINH,x,y,240,15,0xc0c0c0);
				rect(pbuf,WINW,WINH,x,y+15,240,1,0x0);
			}
			if(node->buf)
			{
				l=strlen(node->buf);
				if(l>30)
				{
					l-=30;
				}
				else
				{
					l=0;
				}
				p_str(node->buf+l,strlen(node->buf+l),x,y,0x0,pbuf,WINW,WINH);
			}
			y+=16;
		}
		else if(node->type==E_SUBM)
		{
			if(display_text(node->str,&cx,&x,&y,0xff0000,1,&line))
			{
				ret=1;
				break;
			}
		}
		node=node->next;
	}
	
	if(mode==0)
	{
		rect(pbuf,WINW,WINH,0,24,WINW,24,0xe0e0e0);
		p_str(pages[current_page].pagepath,strlen(pages[current_page].pagepath),4,28,0x0,pbuf,WINW,WINH);
	}
	else
	{
		rect(pbuf,WINW,WINH,0,24,WINW,24,0x00ffff);
		p_str(input_buf,strlen(input_buf),4,28,0x0,pbuf,WINW,WINH);
	}
	mutex_unlock(&page_lock);
	return ret;
}
struct page_element *get_page_element_by_pos(int X,int Y)
{
	struct page_element *node;
	int cx,x,y,ox,oy,line;
	node=page_elements;
	cx=0;
	x=4;
	y=48-current_y;
	line=0;
	while(node)
	{
		ox=x;
		oy=y;
		if(node->type==E_TEXT)
		{
			display_text(node->str,&cx,&x,&y,0x0,0,&line);
		}
		else if(node->type==E_LINK)
		{
			display_text(node->str2,&cx,&x,&y,0x60d0,0,&line);
		}
		else if(node->type==E_NEWL)
		{
			cx=0;
			x=4;
			y+=16;
		}
		else if(node->type==E_INBX)
		{
			x=4;
			y+=16;
			cx=0;
			y+=16;
		}
		else if(node->type==E_SUBM)
		{
			display_text(node->str,&cx,&x,&y,0xff0000,0,&line);
		}
		if(Y>=oy+16&&Y<y)
		{
			return node;
		}
		else if(oy==y)
		{
			if(X>=ox&&X<x&&Y>=y&&Y<y+16)
			{
				return node;
			}
		}
		else
		{
			if(X>=ox&&Y>=oy&&Y<oy+16)
			{
				return node;
			}
			if(X<x&&Y>=y&&Y<y+16)
			{
				return node;
			}
		}
		node=node->next;
	}
	return NULL;
}
int get_page_height(void)
{
	struct page_element *node;
	int cx,x,y,line;
	node=page_elements;
	cx=0;
	x=4;
	y=48;
	line=0;
	while(node)
	{
		if(node->type==E_TEXT)
		{
			display_text(node->str,&cx,&x,&y,0x0,0,&line);
		}
		else if(node->type==E_LINK)
		{
			display_text(node->str2,&cx,&x,&y,0x60d0,0,&line);
		}
		else if(node->type==E_NEWL)
		{
			cx=0;
			x=4;
			y+=16;
		}
		else if(node->type==E_INBX)
		{
			x=4;
			y+=16;
			cx=0;
			y+=16;
		}
		else if(node->type==E_SUBM)
		{
			display_text(node->str,&cx,&x,&y,0xff0000,0,&line);
		}
		node=node->next;
	}
	return y+16;
}
