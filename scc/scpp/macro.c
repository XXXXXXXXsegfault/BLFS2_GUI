struct macro_tab
{
	char *name;
	char *args;
	char *def;
	long int argc;
	long int n;
	struct macro_tab *next;
} *macro_tab[1021];
void macro_tab_add(char *name,char *args,char *def,int argc)
{
	int hash;
	struct macro_tab *node;
	hash=name_hash(name);
	node=xmalloc(sizeof(*node));
	node->name=xstrdup(name);
	node->args=args;
	node->def=xstrdup(def);
	node->argc=argc;
	node->n=0;
	node->next=macro_tab[hash];
	macro_tab[hash]=node;
}
void macro_tab_del(char *name) // call macro_tab_find first
{
	int hash;
	struct macro_tab *node,*p;
	hash=name_hash(name);
	node=macro_tab[hash];
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
		macro_tab[hash]=node->next;
	}
	free(node->name);
	free(node->args);
	free(node->def);
	free(node);
}
struct macro_tab *macro_tab_find(char *name)
{
	int hash;
	struct macro_tab *node;
	hash=name_hash(name);
	node=macro_tab[hash];
	while(node&&strcmp(name,node->name))
	{
		node=node->next;
	}
	return node;
}
char *parse_arglist1(char **str,int *argc)
{
	int n,s;
	char *ret,*word;
	n=0;
	s=0;
	if(**str!='(')
	{
		*argc=0;
		return 0;
	}
	ret=xstrdup("(");
	*str=skip_spaces(*str+1);
	while(word=read_word(str))
	{
		if(!s)
		{
			if(!is_id(word[0]))
			{
				if(n==0&&word[0]==')')
				{
					break;
				}
				else
				{
					error(current_file,current_line,"#define format error.");
				}
			}
			++n;
		}
		else
		{
			if(word[0]==')')
			{
				break;
			}
			else if(word[0]!=',')
			{
				error(current_file,current_line,"#define format error.");
			}
		}
		ret=str_s_app(ret,word);
		free(word);
		s^=1;
		*str=skip_spaces(*str);
	}
	if(word==0)
	{
		error(current_file,current_line,"#define format error.");
	}
	ret=str_s_app(ret,word);
	free(word);
	*argc=n;
	return ret;
}
char **parse_arglist2(char **str,int *argc)
{
	int n,brackets,s;
	char *word;
	char **argv,**t,*arg;
	n=0;
	s=0;
	brackets=1;
	arg=0;
	argv=0;
	++*str;
	while(word=read_word(str))
	{
		if(word[0]==',')
		{
			if(brackets==1)
			{
				t=xmalloc((n+1)*sizeof(void *));
				memcpy(t,argv,n*sizeof(void *));
				t[n]=arg;
				free(argv);
				argv=t;
				arg=0;
				++n;
			}
			else
			{
				arg=str_s_app(arg,word);
			}
		}
		else if(word[0]=='(')
		{
			++brackets;
			arg=str_s_app(arg,word);
		}
		else if(word[0]==')')
		{
			--brackets;
			if(brackets==0)
			{
				t=xmalloc((n+1)*sizeof(void *));
				memcpy(t,argv,n*sizeof(void *));
				t[n]=arg;
				free(argv);
				argv=t;
				if(s)
				{
					++n;
				}
				free(word);
				break;
			}
			else
			{
				arg=str_s_app(arg,word);
			}
		}
		else
		{
			if(is_id(word[0])||word[0]=='\"')
			{
				s=1;
			}
			arg=str_s_app(arg,word);
		}
		free(word);
	}
	*argc=n;
	return argv;
}
int arglist_find(char *arglist,char *str)
{
	int n;
	char *word;
	n=0;
	++arglist;
	arglist=skip_spaces(arglist);
	if(*arglist==')')
	{
		return -1;
	}
	while(word=read_word(&arglist))
	{
		if(!strcmp(word,str))
		{
			free(word);
			return n;
		}
		arglist=skip_spaces(arglist);
		if(*arglist==')')
		{
			free(word);
			break;
		}
		arglist=skip_spaces(arglist+1);
		++n;
		free(word);
	}
	return -1;
}
void add_macro(char *name,char *str)
{
	int argc;
	char *args;
	str=skip_spaces(str)+strlen(name);
	args=parse_arglist1(&str,&argc);
	str=skip_spaces(str);
	macro_tab_add(name,args,str,argc);
}
struct word_list
{
	char *str;
	struct word_list *next;
} *word_head,*word_end;

void word_list_push(char *str)
{
	struct word_list *node;
	node=xmalloc(sizeof(*node));
	node->str=xstrdup(str);
	node->next=0;
	if(word_head)
	{
		word_end->next=node;
	}
	else
	{
		word_head=node;
	}
	word_end=node;
}
char *word_list_release(void)
{
	struct word_list *node;
	char *str;
	str=0;
	while(node=word_head)
	{
		word_head=node->next;
		str=str_s_app(str,node->str);
		free(node->str);
		free(node);
	}
	word_end=0;
	return str;
}
char *do_macro_replace(char *base,char **str)
{
	char *ret;
	struct macro_tab *mtab;
	char *word,*newstr,*ptr;
	char **argv;
	int argc,n;
	ret=0;
	newstr=0;
	if(base)
	{
		ret=xstrdup(base);
	}
	word=read_word(str);
	mtab=macro_tab_find(word);
	if(!mtab||mtab->n)
	{
		ret=str_s_app(ret,word);
		free(word);
		free(base);
		return ret;
	}
	else
	{
		mtab->n=1;
		if(mtab->args)
		{
			*str=skip_spaces(*str);
			if(**str!='(')
			{
				newstr=str_s_app(newstr,word);
				free(word);
			}
			else
			{
				argc=0;
				argv=parse_arglist2(str,&argc);
				if(argc!=mtab->argc)
				{
					error(current_file,current_line,"numbers of arguments did not match.");
				}
				free(word);
				ptr=mtab->def;
				while(word=read_word(&ptr))
				{
					n=arglist_find(mtab->args,word);
					if(n!=-1)
					{
						newstr=str_s_app(newstr,argv[n]);
					}
					else
					{
						newstr=str_s_app(newstr,word);
					}
					free(word);
				}
				n=0;
				while(n<argc)
				{
					free(argv[n]);
					++n;
				}
				free(argv);
			}
		}
		else
		{
			newstr=xstrdup(mtab->def);
			free(word);
		}
		if(ptr=newstr)
		{
			while(*ptr)
			{
				ret=do_macro_replace(ret,&ptr);
			}
		}
		free(base);
		mtab->n=0;
		return ret;
	}
}
void macro_replace(void)
{
	char *str,*ret,*ptr;
	int status;
	str=word_list_release();
	do
	{
		ptr=str;
		ret=0;
		while(*ptr)
		{
			ret=do_macro_replace(ret,&ptr);
		}
		status=strcmp(ret,str);
		free(str);
		str=ret;
	}
	while(status);
	c_write(ret,strlen(ret));
	free(ret);
}
