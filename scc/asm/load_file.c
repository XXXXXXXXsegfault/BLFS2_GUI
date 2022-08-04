long int current_line;
int readc(void)
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
char *read_line(void)
{
	char *str;
	char c;
	str=0;
	while((c=readc())!=-1)
	{
		if(c=='\n')
		{
			if(str==0)
			{
				str=xstrdup(" ");
			}
			break;
		}
		str=str_c_app(str,c);
	}
	++current_line;
	return str;
}
struct lines
{
	char *str;
	long int line;
	unsigned long int ins_pos;
	unsigned long int prev_ins_pos;
	int needs_recompile;
	int ins_len;
	char *ins_buf;
	unsigned long int ins_off;
	unsigned long int optimize;
	struct lines *next;
} *lines_head,*lines_end;
void load_file(void)
{
	char *str;
	struct lines *node;
	while(str=read_line())
	{
		node=xmalloc(sizeof(*node));
		node->str=str;
		node->line=current_line;
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
