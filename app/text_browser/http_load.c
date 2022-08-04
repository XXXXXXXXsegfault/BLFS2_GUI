void f_read_line(int fd,char *buf)
{
	int x;
	char c;
	x=0;
	while(read(fd,&c,1)==1)
	{
		if(x<4095)
		{
			buf[x]=c;
			++x;
		}
		if(c=='\n')
		{
			break;
		}
	}
	buf[x]=0;
}
struct page *http_page_ptr;
int page_load(char *path,struct page *page);
int http_parse(int in,int out)
{
	char buf[4096];
	char c;
	int x,size;
	int encoding;
	encoding=0;
	while(1)
	{
		f_read_line(in,buf);
		if(!strcmp(buf,"\r\n"))
		{
			break;
		}
		if(!strncmp(buf,"Transfer-Encoding: ",19))
		{
			x=19;
			while(buf[x])
			{
				if(buf[x]=='c')
				{
					if(!strncmp(buf+x,"chunked",7))
					{
						encoding=1;
					}
				}
				++x;
			}
		}
		if(!strncmp(buf,"Location: ",10))
		{
			x=strlen(buf);
			buf[x-2]=0;
			return page_load(buf+10,http_page_ptr)|2;
		}
	}
	if(encoding)
	{
		while(1)
		{
			x=0;
			size=0;
			c=0;
			while(read(in,&c,1)==1)
			{
				if(c>='0'&&c<='9')
				{
					size=size*16+c-'0';
				}
				else if(c>='A'&&c<='F')
				{
					size=size*16+c-'A'+10;
				}
				else if(c>='a'&&c<='f')
				{
					size=size*16+c-'a'+10;
				}
				else
				{
					break;
				}
			}
			if(size==0)
			{
				break;
			}
			read(in,buf,1);
			while(size)
			{
				if(size>256)
				{
					read(in,buf,256);
					write(out,buf,256);
					size-=256;
				}
				else
				{
					read(in,buf,size);
					write(out,buf,size);
					size=0;
				}
			}
			read(in,buf,2);
		}
	}
	else
	{
		while((x=read(in,buf,256))>0)
		{
			write(out,buf,x);
		}
	}
	return 0;
}
int http_load(char *prog,char *path,struct page *page)
{
	unsigned char name[40];
	static char buf[256];
	int fd,fdo,pid,status;
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
	strcpy(buf,tmp_path);
	strcat(buf,"/page.http");
	pid=fork();
	if(pid<0)
	{
		return 1;
	}
	if(pid==0)
	{
		char *argv[4];
		argv[0]=prog;
		argv[1]=path;
		argv[2]=buf;
		argv[3]=0;
		execv(prog,argv);
		exit(1);
	}
	status=0;
	waitpid(pid,&status,0);
	if(status)
	{
		return 1;
	}
	fd=open(buf,0,0);
	if(fd<0)
	{
		return 1;
	}
	fdo=openat(tmpfd,name,578|0200,0644);
	if(fdo<0)
	{
		close(fd);
		return 1;
	}
	http_page_ptr=page;
	x=http_parse(fd,fdo);
	close(fd);
	close(fdo);
	if(x<2)
	{
		memcpy(page->pagename,name,40);
	}
	return x;
}
