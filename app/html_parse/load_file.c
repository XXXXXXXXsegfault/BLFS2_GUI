unsigned char file_ungetc_buf[11];
int file_getc(void)
{
	static unsigned char buf[4096];
	static int x,size;
	unsigned int ret;
	unsigned int x1;
	if(x1=file_ungetc_buf[10])
	{
		ret=file_ungetc_buf[x1-1];
		file_ungetc_buf[10]=x1-1;
		return ret;
	}
	if(x==size)
	{
		x=0;
		size=read(fdi,buf,4096);
		if(size<=0)
		{
			size=0;
			return -1;
		}
	}
	ret=buf[x];
	++x;
	if((ret<32||ret>126)&&ret!='\n'&&ret!='\r'&&ret!='\t'&&ret!='\v')
	{
		ret='\?';
	}
	return ret;
}
void file_ungetc(int c)
{
	unsigned int x;
	x=file_ungetc_buf[10];
	file_ungetc_buf[x]=c;
	file_ungetc_buf[10]=x+1;
}
struct token
{
	char *str;
	struct token *next;
} *token;
char *file_get_str(char *str,char term)
{
	int c;
	int n;
	n=strlen(str);
	while((c=file_getc())!=-1)
	{
		str=str_c_app(str,c,n);
		if(c==term)
		{
			break;
		}
		if(c=='\\')
		{
			c=file_getc();
			if(c==-1)
			{
				break;
			}
			str=str_c_app(str,c,n);
			++n;
		}
		++n;
	}
	return str;
}
char *file_read_tag(void)
{
	char *ret;
	int c,n;
	ret=NULL;
	n=0;
	while((c=file_getc())!=-1)
	{
		ret=str_c_app(ret,c,n);
		++n;
		if(c=='>')
		{
			break;
		}
		else if(c=='\"')
		{
			ret=file_get_str(ret,'\"');
			n=strlen(ret);
		}
		else if(c=='\'')
		{
			ret=file_get_str(ret,'\'');
			n=strlen(ret);
		}
	}
	return ret;
}
char *file_read_text(void)
{
	char *ret;
	int c,n;
	ret=NULL;
	n=0;
	while((c=file_getc())!=-1)
	{
		if(c=='<')
		{
			file_ungetc(c);
			break;
		}
		ret=str_c_app(ret,c,n);
		++n;
	}
	return ret;
}
int is_comment(void)
{
	int c[3];
	c[0]=file_getc();
	if(c[0]==-1)
	{
		return 0;
	}
	c[1]=file_getc();
	if(c[1]==-1)
	{
		file_ungetc(c[0]);
		return 0;
	}
	c[2]=file_getc();
	if(c[2]==-1)
	{
		file_ungetc(c[1]);
		file_ungetc(c[0]);
		return 0;
	}
	if(c[0]!='!'||c[1]!='-'||c[2]!='-')
	{
		file_ungetc(c[2]);
		file_ungetc(c[1]);
		file_ungetc(c[0]);
		return 0;
	}
	return 1;
}
char *file_read_token(void)
{
	char *ret;
	int c;
	int n,n1;
	unsigned char *end_comment;
	c=file_getc();
	n1=0;
	if(c==-1)
	{
		return NULL;
	}
	if(c=='<')
	{
		if(is_comment())
		{
			ret=xstrdup("<!--");
			end_comment="-->";
			n=0;
			while((c=file_getc())!=-1)
			{
				ret=str_c_app(ret,c,n1);
				if(c==end_comment[n])
				{
					++n;
					if(n==3)
					{
						break;
					}
				}
				else
				{
					n=0;
				}
				++n1;
			}
			return ret;
		}
		file_ungetc(c);
		return file_read_tag();
	}
	file_ungetc(c);
	return file_read_text();
}
void load_file(void)
{
	struct token *node,*end;
	char *str;
	end=NULL;
	while(str=file_read_token())
	{
		node=xmalloc(sizeof(*node));
		node->str=str;
		node->next=node;
		if(end)
		{
			end->next=node;
		}
		else
		{
			token=node;
		}
		end=node;
	}
}
