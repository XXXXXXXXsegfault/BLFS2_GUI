struct token *locate_tag_end(struct token *tag)
{
	char *str,*p;
	int n;
	str=xstrdup("</");
	p=tag->str+1;
	n=0;
	while(*p&&*p!=32&&*p!='/'&&*p!='>')
	{
		str=str_c_app(str,*p,n);
		++p;
		++n;
	}
	str=str_c_app(str,'>',n);
	while(tag->next!=tag)
	{
		if(!strcmp(tag->str,str))
		{
			break;
		}
		tag=tag->next;
	}
	if(!strcmp(tag->str,str))
	{
		free(str);
		return tag;
	}
	free(str);
	return NULL;
}
int get_special_character(char **str)
{
	if(!strncmp(*str,"nbsp;",5))
	{
		*str+=5;
		return ' ';
	}
	else if(!strncmp(*str,"lt;",3))
	{
		*str+=3;
		return '<';
	}
	else if(!strncmp(*str,"gt;",3))
	{
		*str+=3;
		return '>';
	}
	else if(!strncmp(*str,"amp;",4))
	{
		*str+=4;
		return '&';
	}
	else
	{
		while(**str&&**str!=';')
		{
			++*str;
		}
		++*str;
		return '\?';
	}
}
void out_text(char *str)
{
	char buf[257];
	char c;
	int x;
	x=0;
	while(c=*str)
	{
		if(c=='&')
		{
			++str;
			buf[x]=get_special_character(&str);
		}
		else
		{
			buf[x]=c;
			++str;
		}
		++x;
		if(x==256)
		{
			x=0;
			write(fdo,buf,256);
		}
	}
	buf[x]=0;
	write(fdo,buf,x+1);
}
char *get_property(char *tag,char *name)
{
	int l,c;
	while(*tag&&*tag!=32)
	{
		++tag;
	}
	while(*tag==32)
	{
		++tag;
	}
	l=strlen(name);
	while(*tag)
	{
		if(tag[0]==name[0])
		{
			if(!strncmp(tag,name,l))
			{
				break;
			}
		}
		else if(*tag=='\"')
		{
			++tag;
			while((c=*tag)&&c!='\"')
			{
				++tag;
				if(c=='\\')
				{
					++tag;
				}
			}
			if(!*tag)
			{
				break;
			}
		}
		++tag;
	}
	if(!*tag)
	{
		return NULL;
	}
	tag+=l;
	if(*tag!='=')
	{
		return NULL;
	}
	++tag;
	while(*tag&&*tag=='\n'||*tag=='\t'||*tag=='\v'||*tag=='\r'||*tag==32)
	{
		++tag;
	}
	return tag;
}
void out_property(char *tag,char *name)
{
	int x,c;
	char buf[257];
	tag=get_property(tag,name);
	if(tag==NULL)
	{
		write(fdo,"ERROR",6);
		return;
	}
	if(*tag!='\"'&&*tag!='\'')
	{
		x=0;
		while((c=*tag)&&c!=32&&c!='>')
		{
			++tag;
			if(c=='\\')
			{
				c=*tag;
				++tag;
			}
			else if(c=='&')
			{
				c=get_special_character(&tag);
			}
			buf[x]=c;
			++x;
			if(x==256)
			{
				x=0;
				write(fdo,buf,256);
			}
		}
		buf[x]=0;
		write(fdo,buf,x+1);
		return;
	}
	else if(*tag=='\"')
	{
		++tag;
		x=0;
		while((c=*tag)&&c!='\"')
		{
			++tag;
			if(c=='\\')
			{
				c=*tag;
				++tag;
			}
			else if(c=='&')
			{
				c=get_special_character(&tag);
			}
			buf[x]=c;
			++x;
			if(x==256)
			{
				x=0;
				write(fdo,buf,256);
			}
		}
		buf[x]=0;
	}
	else
	{
		++tag;
		x=0;
		while((c=*tag)&&c!='\'')
		{
			++tag;
			if(c=='\\')
			{
				c=*tag;
				++tag;
			}
			else if(c=='&')
			{
				c=get_special_character(&tag);
			}
			buf[x]=c;
			++x;
			if(x==256)
			{
				x=0;
				write(fdo,buf,256);
			}
		}
		buf[x]=0;
	}
	write(fdo,buf,x+1);
}
#include "tags.c"
