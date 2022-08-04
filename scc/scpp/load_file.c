long int condc,condc_levels,old_condc_levels,condc_else;
struct macro_tab2
{
	char *name;
	struct macro_tab2 *next;
} *macro_tab2[1021];
void macro_tab2_add(char *name)
{
	int hash;
	struct macro_tab2 *node;
	hash=name_hash(name);
	node=xmalloc(sizeof(*node));
	node->name=xstrdup(name);
	node->next=macro_tab2[hash];
	macro_tab2[hash]=node;
}
void macro_tab2_del(char *name)
{
	int hash;
	struct macro_tab2 *node,*p;
	hash=name_hash(name);
	node=macro_tab2[hash];
	p=0;
	while(node&&strcmp(name,node->name))
	{
		p=node;
		node=node->next;
	}
	if(p)
	{
		p->next=node->next;
	}
	else
	{
		macro_tab2[hash]=node->next;
	}
	free(node->name);
	free(node);
}
struct macro_tab2 *macro_tab2_find(char *name)
{
	int hash;
	struct macro_tab2 *node;
	hash=name_hash(name);
	node=macro_tab2[hash];
	while(node&&strcmp(name,node->name))
	{
		node=node->next;
	}
	return node;
}
char *get_ppcmd(char *str)
{
	skip_spaces(str);
	if(*str!='#')
	{
		return 0;
	}
	++str;
	skip_spaces(str);
	return str;
}
char *sgetc(char *str,char *ret)
{
	int x;
	if(str[0]=='\\')
	{
		if(str[1]=='\\')
		{
			*ret='\\';
			return str+2;
		}
		else if(str[1]=='n')
		{
			*ret='\n';
			return str+2;
		}
		else if(str[1]=='t')
		{
			*ret='\t';
			return str+2;
		}
		else if(str[1]=='v')
		{
			*ret='\v';
			return str+2;
		}
		else if(str[1]=='r')
		{
			*ret='\r';
			return str+2;
		}
		else if(str[1]=='\'')
		{
			*ret='\'';
			return str+2;
		}
		else if(str[1]=='\"')
		{
			*ret='\"';
			return str+2;
		}
		else if(str[1]=='\?')
		{
			*ret='\?';
			return str+2;
		}
		else if(str[1]>='0'&&str[1]<='7')
		{
			x=1;
			*ret=0;
			while(str[x]>='0'&&str[x]<='7')
			{
				*ret=(*ret<<3)+(str[x]-'0');
				++x;
			}
			return str+x;
		}
		else if(str[1]=='x')
		{
			x=2;
			*ret=0;
			while(1)
			{
				if(str[x]>='0'&&str[x]<='9')
				{
					*ret=*ret*16+(str[x]-'0');
				}
				else if(str[x]>='A'&&str[x]<='F')
				{
					*ret=*ret*16+(str[x]-'A'+10);
				}
				else if(str[x]>='a'&&str[x]<='f')
				{
					*ret=*ret*16+(str[x]-'a'+10);
				}
				else
				{
					break;
				}
				++x;
			}
			return str+x;
		}
		else
		{
			*ret='\\';
			return str+1;
		}
	}
	else
	{
		*ret=str[0];
		return str+1;
	}
}
char *dir_name(char *str)
{
	long int x,x1;
	char *ret;
	x=strlen(str);
	if(x==0)
	{
		return xstrdup(".");
	}
	ret=xstrdup(str);
	while(x&&ret[x-1]=='/')
	{
		ret[x-1]=0;
		--x;
	}
	x=0;
	if(ret[0]=='/')
	{
		x=1;
	}
	x1=x;
	while(ret[x])
	{
		if(ret[x]=='/')
		{
			x1=x;
			while(ret[x]=='/')
			{
				++x;
			}
		}
		else
		{
			++x;
		}
	}
	ret[x1]=0;
	if(ret[0]==0)
	{
		ret[0]='.';
		ret[1]=0;
	}
	return ret;
}
void load_file(int dir_fd,char *name,int fd,int check);

void do_include(int dir_fd,char *name,long int line,char *str)
{
	char c;
	char *fname,*dname;
	int new_dir_fd,new_fd;
	fname=0;
	str=skip_spaces(str);
	if(*str=='\"')
	{
		if(include_level==128)
		{
			error(name,line,"too many levels of #include.");
		}
		++include_level;
		++str;
		while(*str&&*str!='\"')
		{
			str=sgetc(str,&c);
			fname=str_c_app(fname,c);
		}
		dname=dir_name(name);
		new_dir_fd=openat(dir_fd,dname,0,0);
		if(new_dir_fd<0)
		{
			error(name,line,"cannot open file.");
		}
		new_fd=openat(new_dir_fd,fname,0,0);
		if(new_fd<0)
		{
			error(name,line,"cannot open file.");
		}
		load_file(new_dir_fd,fname,new_fd,0);
		close(new_dir_fd);
		close(new_fd);
		free(fname);
		free(dname);
		--include_level;
	}
	else
	{
		error(name,line,"expected file name in #include.");
	}
}
void load_file(int dir_fd,char *name,int fd,int check)
{
	long int flines;
	long int old_current_line;
	char *str,*str1;
	struct lines_list *node;
	long int len;
	char *word;
	flines=current_line;
	while(str=read_line(fd))
	{
		if((str1=get_ppcmd(str)))
		{
			if(!strncmp(str1,"include",7))
			{
				if(!condc)
				{
					old_current_line=current_line;
					do_include(dir_fd,name,current_line-flines,str1+7);
					flines+=current_line-old_current_line;
				}
			}
			else if(!strncmp(str1,"error",5))
			{
				if(!condc)
				{
					error(name,current_line-flines,str1+6);
				}
			}
			else if(!strncmp(str1,"define",6))
			{
				if(!condc)
				{
					str1=skip_spaces(str1+7);
					word=read_word(&str1);
					if(macro_tab2_find(word))
					{
						error(name,current_line-flines,"macro redefined.");
					}
					macro_tab2_add(word);
					free(word);
				}
			}
			else if(!strncmp(str1,"undef",5))
			{
				if(!condc)
				{
					str1=skip_spaces(str1+6);
					word=read_word(&str1);
					if(!macro_tab2_find(word))
					{
						error(name,current_line-flines,"macro not defined.");
					}
					macro_tab2_del(word);
					free(word);
				}
			}
			else if(!strncmp(str1,"ifdef",5))
			{
				str1=skip_spaces(str1+6);
				word=read_word(&str1);
				++condc_levels;
				if(!condc&&!macro_tab2_find(word))
				{
					condc=1;
					old_condc_levels=condc_levels;
				}
				free(word);
			}
			else if(!strncmp(str1,"ifndef",6))
			{
				str1=skip_spaces(str1+7);
				word=read_word(&str1);
				++condc_levels;
				if(!condc&&macro_tab2_find(word))
				{
					condc=1;
					old_condc_levels=condc_levels;
				}
				free(word);
			}
			else if(!strncmp(str1,"endif",5))
			{
				if(!condc_levels)
				{
					error(name,current_line-flines,"#endif without #ifdef or #ifndef.");
				}
				if(old_condc_levels==condc_levels)
				{
					old_condc_levels=0;
					condc=0;
					condc_else=0;
				}
				--condc_levels;
			}
			else if(!strncmp(str1,"else",4))
			{
				if(!condc_levels||condc_else)
				{
					error(name,current_line-flines,"#else without #ifdef or #ifndef.");
				}
				if(condc==1&&old_condc_levels==condc_levels)
				{
					condc=0;
				}
				else if(condc==0)
				{
					old_condc_levels=condc_levels;
					condc=1;
					condc_else=1;
				}
			}
			else if(!condc)
			{
				error(name,current_line-flines,"unknown preprocessor command.");
			}
		}
		if(!condc)
		{
			node=xmalloc(sizeof(*node));
			node->line=current_line;
			node->fline=current_line-flines;
			node->fname=xstrdup(name);
			node->str=str;
			node->next=0;
			if(lines_head)
			{
				lines_end->next=node;
			}
			else
			{
				lines_head=node;
			}
			lines_end=node;
		}
	}
	if(check&&condc_levels)
	{
		error(name,current_line-flines,"#endif not found at end of file.");
	}
}
