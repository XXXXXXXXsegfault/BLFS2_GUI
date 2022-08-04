#include "../include/lib.c"
long int current_line;
int fdi,fdo;
int include_level;
int is_comment;
char *current_file;
long int macro_state;
struct lines_list
{
	char *str;
	long int line;
	long int fline;
	char *fname;
	struct lines_list *next;
} *lines_head,*lines_end;

void error(char *file,int line,char *msg)
{
	char *str;
	str=xstrdup("file \"");
	str=str_s_app(str,file);
	str=str_s_app(str,"\" line ");
	str=str_i_app(str,line);
	str=str_s_app(str,": error: ");
	str=str_s_app(str,msg);
	str=str_c_app(str,'\n');
	write(2,str,strlen(str));
	exit(2);
}
int name_hash(char *str)
{
	unsigned int hash;
	hash=0;
	while(*str)
	{
		hash=(hash<<11|hash>>21)+*str;
		++str;
	}
	return hash%1021;
}
char outc_buf[65536];
int outc_x;
void outc(char c)
{
	if(outc_x==65536)
	{
		write(fdo,outc_buf,outc_x);
		outc_x=0;
	}
	outc_buf[outc_x]=c;
	++outc_x;
}
void out_flush(void)
{
	if(outc_x)
	{
		write(fdo,outc_buf,outc_x);
	}
}
void c_write(char *buf,int size)
{
	while(size)
	{
		outc(*buf);
		++buf;
		--size;
	}
}
char *read_line(int fd)
{
	char *str;
	char c;
	long int x;
	str=0;
	while(read(fd,&c,1)==1)
	{
		if(c=='\\')
		{
			if(read(fd,&c,1)==1)
			{
				if(c=='\n')
				{
					++current_line;
				}
				else
				{
					str=str_c_app(str,'\\');
					str=str_c_app(str,c);
				}
			}
			else
			{
				break;
			}
		}
		else if(c=='\n')
		{
			if(str==0)
			{
				str=xstrdup(" ");
			}
			break;
		}
		else
		{
			str=str_c_app(str,c);
		}
	}
	if(str==0)
	{
		return 0;
	}
	x=0;
	while(c=str[x])
	{
		if(is_comment)
		{
			str[x]=32;
			if(c=='*'&&str[x+1]=='/')
			{
				str[x+1]=32;
				is_comment=0;
				++x;
			}
		}
		else if(c=='/')
		{
			if(str[x+1]=='/')
			{
				str[x]=0;
				break;
			}
			else if(str[x+1]=='*')
			{
				str[x]=32;
				str[x+1]=32;
				++x;
				is_comment=1;
			}
		}
		else if(c=='\'')
		{
			do
			{
				++x;
				c=str[x];
				if(c==0)
				{
					break;
				}
				else if(c=='\\')
				{
					++x;
					if(str[x]==0)
					{
						break;
					}
				}
			}
			while(c!='\'');
		}
		else if(c=='\"')
		{
			do
			{
				++x;
				c=str[x];
				if(c==0)
				{
					break;
				}
				else if(c=='\\')
				{
					++x;
					if(str[x]==0)
					{
						break;
					}
				}
			}
			while(c!='\"');
		}
		++x;
	}
	++current_line;
	return str;
}
#include "read_word.c"
#include "load_file.c"
#include "macro.c"
int main(int argc,char **argv)
{
	struct lines_list *l;
	char *ptr,*word,*cmd,*t;
	struct macro_tab *mtab;
	if(argc<3)
	{
		return 1;
	}
	fdi=open(argv[1],0,0);
	if(fdi<0)
	{
		return 1;
	}
	fdo=open(argv[2],578,0644);
	if(fdo<0)
	{
		return 1;
	}
	load_file(-100,argv[1],fdi,1);
	l=lines_head;
	while(l)
	{
		ptr=l->str;
		current_line=l->fline;
		current_file=l->fname;
		if(cmd=get_ppcmd(ptr))
		{
			if(!strncmp(cmd,"define",6))
			{
				t=skip_spaces(cmd+7);
				word=read_word(&t);
				if(!word||!is_id(word[0]))
				{
					error(l->fname,l->fline,"expected macro name after #define.");
				}
				add_macro(word,cmd+7);
				free(word);
			}
			else if(!strncmp(cmd,"undef",5))
			{
				t=skip_spaces(cmd+6);
				word=read_word(&t);
				if(!word||!is_id(word[0]))
				{
					error(l->fname,l->fline,"expected macro name after #undef.");
				}
				macro_tab_del(word);
				free(word);
			}
		}
		else
		{
			while(word=read_word(&ptr))
			{
				if(macro_state)
				{
					if(macro_state==1)
					{
						if(word[0]=='(')
						{
							word_list_push(word);
							macro_state=2;
						}
						else
						{
							t=word_list_release();
							c_write(t,strlen(t));
							c_write(word,strlen(word));
							free(t);
							macro_state=0;
						}
					}
					else
					{
						word_list_push(word);
						if(word[0]=='(')
						{
							++macro_state;
						}
						else if(word[0]==')')
						{
							--macro_state;
							if(macro_state==1)
							{
								macro_state=0;
								macro_replace();
							}
						}
					}
				}
				else if(mtab=macro_tab_find(word))
				{
					word_list_push(word);
					if(mtab->args)
					{
						macro_state=1;
					}
					else
					{
						macro_replace();
					}
				}
				else
				{
					c_write(word,strlen(word));
				}
				free(word);
			}
			c_write("\n",1);
		}
		l=l->next;
	}
	out_flush();
	return 0;
}
