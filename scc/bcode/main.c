#include "../include/lib.c"
int fdi,fdo;
long int current_line;
void error(int line,char *msg)
{
	char *str;
	str=xstrdup("line ");
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
long int slen(char *str)
{
	int c;
	long int l;
	l=0;
	if(*str=='\"')
	{
		l=1;
		while(str[l])
		{
			if(str[l]=='\"')
			{
				++l;
				break;
			}
			if(str[l]=='\\')
			{
				++l;
				if(str[l]==0)
				{
					error(current_line,"string not complete.");
				}
			}
			++l;
		}
		return l;
	}
	while(c=*str)
	{
		if(c==32||c=='\t'||c=='\v'||c=='\r')
		{
			break;
		}
		++l;
		++str;
	}
	return l;
}
char *snext(char *str)
{
	long int l;
	if(!str)
	{
		return 0;
	}
	l=slen(str);
	str+=l;
	while(*str==32||*str=='\t'||*str=='\v'||*str=='\r')
	{
		++str;
	}
	if(*str==0)
	{
		return 0;
	}
	return str;
}
char *sdup(char *str)
{
	char *ret;
	long int l;
	if(str==0)
	{
		return 0;
	}
	l=slen(str);
	if(l==0)
	{
		return 0;
	}
	ret=xmalloc(l+1);
	memcpy(ret,str,l);
	ret[l]=0;
	return ret;
}
#include "out.c"
#include "fg.c"
#include "gencode.c"

int main(int argc,char **argv)
{
	char *str;
	struct ins *node;
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
	while(str=read_line())
	{
		ins_add(str);
		free(str);
	}
	load_global_vars();
	load_labels();
	load_branches();
	load_local_vars();
	reg_init();
	write_msg();
	node=ins_head;
	while(node)
	{
		gen_code(node);
		node=node->next;
	}

	outs(".datasize ");
	out_num64(data_size);
	outs("\n");
	out_flush();
	return 0;
}
