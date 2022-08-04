int l_ungetc_buf;
int l_current_line;
int l_current_col;
int l_old_line;
int l_old_col;
int l_readc(void)
{
	static unsigned char buf[65536];
	static int x,n;
	int n1,c;
	if(x==n)
	{
		n1=read(fdi,buf,65536);
		if(n1<=0)
		{
			return -1;
		}
		n=n1;
		x=0;
	}
	c=buf[x];
	++x;
	return c;
}
int l_getc(void)
{
	int c;
	c=0;
	if(l_ungetc_buf!=-1)
	{
		c=l_ungetc_buf;
		l_ungetc_buf=-1;
		l_old_line=l_current_line;
		l_old_col=l_current_col;
		if(c=='\n')
		{
			++l_current_line;
			l_current_col=1;
		}
		else
		{
			++l_current_col;
		}
		return c;
	}
	if((c=l_readc())!=-1)
	{
		l_old_line=l_current_line;
		l_old_col=l_current_col;
		if(c=='\n')
		{
			++l_current_line;
			l_current_col=1;
		}
		else
		{
			++l_current_col;
		}
		return c;
	}
	else
	{
		return -1;
	}
}
void l_ungetc(int c)
{
	l_ungetc_buf=c&0xff;
	l_current_line=l_old_line;
	l_current_col=l_old_col;
}
char *read_str(char c)
{
	char *s;
	char c1;
	int line,col;
	s=0;
	line=l_current_line;
	col=l_current_col;
	s=str_c_app(s,c);
	while((c1=l_getc())!=-1)
	{
		s=str_c_app(s,c1);
		if(c1==c)
		{
			break;
		}
		if(c1=='\\')
		{
			c1=l_getc();
			if(c1==-1)
			{
				break;
			}
			s=str_c_app(s,c1);
		}
	}
	if(c1==-1)
	{
		error(line,col,"string not complete.");
	}
	return s;
}
char *l_read_word(void)
{
	char *s;
	int c;
	int line,col;
	char *msg;
	s=0;
	line=l_current_line;
	col=l_current_col;
	while((c=l_getc())!=-1)
	{
		if(c!=32&&c!='\n'&&c!='\t'&&c!='\v'&&c!='\r')
		{
			break;
		}
	}
	if(c==-1)
	{
		return 0;
	}
	if(c>='A'&&c<='Z'||c>='a'&&c<='z'||c>='0'&&c<='9'||c=='_')
	{
		s=str_c_app(s,c);
		c=l_getc();
		while(c>='A'&&c<='Z'||c>='a'&&c<='z'||c>='0'&&c<='9'||c=='_')
		{
			s=str_c_app(s,c);
			c=l_getc();
		}
		if(c!=-1)
		{
			l_ungetc(c);
		}
		return s;
	}
	if(c=='\'')
	{
		return read_str('\'');
	}
	if(c=='\"')
	{
		return read_str('\"');
	}
	s=str_c_app(s,c);
	if(c=='-')
	{
		c=l_getc();
		if(c=='>'||c=='='||c=='-')
		{
			s=str_c_app(s,c);
		}
		else if(c!=-1)
		{
			l_ungetc(c);
		}
	}
	else if(c=='+')
	{
		c=l_getc();
		if(c=='+'||c=='=')
		{
			s=str_c_app(s,c);
		}
		else if(c!=-1)
		{
			l_ungetc(c);
		}
	}
	else if(c=='<')
	{
		c=l_getc();
		if(c=='=')
		{
			s=str_c_app(s,c);
		}
		else if(c=='<')
		{
			s=str_c_app(s,c);
			c=l_getc();
			if(c=='=')
			{
				s=str_c_app(s,c);
			}
			else if(c!=-1)
			{
				l_ungetc(c);
			}
		}
		else if(c!=-1)
		{
			l_ungetc(c);
		}
	}
	else if(c=='>')
	{
		c=l_getc();
		if(c=='=')
		{
			s=str_c_app(s,c);
		}
		else if(c=='>')
		{
			s=str_c_app(s,c);
			c=l_getc();
			if(c=='=')
			{
				s=str_c_app(s,c);
			}
			else if(c!=-1)
			{
				l_ungetc(c);
			}
		}
		else if(c!=-1)
		{
			l_ungetc(c);
		}
	}
	else if(c=='=')
	{
		c=l_getc();
		if(c=='=')
		{
			s=str_c_app(s,c);
		}
		else if(c!=-1)
		{
			l_ungetc(c);
		}
	}
	else if(c=='!')
	{
		c=l_getc();
		if(c=='=')
		{
			s=str_c_app(s,c);
		}
		else if(c!=-1)
		{
			l_ungetc(c);
		}
	}
	else if(c=='&')
	{
		c=l_getc();
		if(c=='&'||c=='=')
		{
			s=str_c_app(s,c);
		}
		else if(c!=-1)
		{
			l_ungetc(c);
		}
	}
	else if(c=='|')
	{
		c=l_getc();
		if(c=='|'||c=='=')
		{
			s=str_c_app(s,c);
		}
		else if(c!=-1)
		{
			l_ungetc(c);
		}
	}
	else if(c=='/'||c=='*'||c=='%'||c=='^')
	{
		c=l_getc();
		if(c=='=')
		{
			s=str_c_app(s,c);
		}
		else if(c!=-1)
		{
			l_ungetc(c);
		}
	}
	else if(!(c=='['||c==']'||c=='('||c==')'||c=='{'||c=='}'||c=='.'||c=='~'||c=='\?'||c==':'||c==','||c==';'))
	{
		msg=xstrdup("unrecognized character \'");
		msg=str_c_app(msg,c);
		msg=str_c_app(msg,'\'');
		error(line,col,msg);
	}
	return s;
}
struct l_word_list
{
	char *str;
	int line;
	int col;
	struct l_word_list *next;
} *l_words_head,*l_words_end;
void load_file(void)
{
	char *s;
	struct l_word_list *node;
	int line,col;
	line=l_current_line;
	col=l_current_col;
	while(s=l_read_word())
	{
		node=xmalloc(sizeof(*node));
		node->str=s;
		node->line=line;
		node->col=col;
		node->next=0;
		if(l_words_head)
		{
			l_words_end->next=node;
		}
		else
		{
			l_words_head=node;
		}
		l_words_end=node;
		line=l_current_line;
		col=l_current_col;
	}
}
void l_global_init(void)
{
	l_ungetc_buf=-1;
	l_current_line=1;
	l_current_col=1;
	l_old_line=1;
	l_old_col=1;
}
