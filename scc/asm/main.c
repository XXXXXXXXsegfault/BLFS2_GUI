#include "../include/lib.c"
int fdi,fdo,fde;
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
	hash=20000;
	while(*str)
	{
		hash=(hash<<11|hash>>21)+*str;
		++str;
	}
	return hash%1021;
}
unsigned long int pc,data_size,data_addr;
struct lines *l;
int stage;
#define ELF_OFF 0x100b0
#include "load_file.c"
#include "elf.c"
#include "read_word.c"
int format_hash(char *format)
{
	char c;
	int brackets;
	unsigned int hash;
	brackets=0;
	hash=300;
	while(c=*format)
	{
		if(c=='(')
		{
			++brackets;
		}
		else if(c==')')
		{
			--brackets;
		}
		else if(!brackets)
		{
			if(c=='*')
			{
				hash=hash*97+31;
			}
			else if(c==',')
			{
				hash=hash*89+19;
			}
			else if(c=='%')
			{
				hash=hash*47+173;
			}
		}
		++format;
	}
	return hash%1021;
}
#include "label.c"
#include "ins/ins.c"
#include "pseudo_op.c"
int main(int argc,char **argv)
{
	char *str,*word;
	struct label *label;
	int s,n;
	struct lines *l_head,*l_end;
	long int ins_size;
	if(argc<3)
	{
		return 1;
	}
	fdi=open(argv[1],0,0);
	if(fdi<0)
	{
		return 1;
	}
	fdo=open(argv[2],578,0755);
	if(fdo<0)
	{
		return 1;
	}
	if(argc>=4)
	{
		fde=open(argv[3],578,0644);
	}
	else
	{
		fde=-1;
	}
	data_addr=0x20000000;
	load_file();
	elf_header.entry=0;
	pc=ELF_OFF;
	ins_init();
	l_head=0;
	l_end=0;
	ins_size=0;
	spos=0;
	l=lines_head;
	while(l)
	{
		str=skip_spaces(l->str);
		l->ins_len=0;
		l->ins_buf=0;
		l->needs_recompile=0;
		if(*str&&*str!='#')
		{
			if(*str=='.')
			{
				parse_pseudo_op(str+1);
			}
			else if(*str=='@')
			{
				++str;
				word=read_word(&str);
				if(label_tab_find(word))
				{
					error(l->line,"label redefined.");
				}
				label_tab_add(word);
				l->needs_recompile=1;
			}
			else
			{
				ins_write(str);
			}
		}
		l=l->next;
	}
	stage=1;
	do
	{
		s=0;
		n=0;
		spos=0;
		elf_header.entry=0;
		pc=ELF_OFF;
		l=lines_head;
		while(l)
		{
			l->ins_pos=pc;
			if(l->needs_recompile)
			{
				str=skip_spaces(l->str);
				if(*str=='@')
				{
					++str;
					word=read_word(&str);
					if(label=label_tab_find(word))
					{
						if(label->value!=pc)
						{
							label->value=pc;
							s=1;
						}
					}
					free(word);
				}
				else
				{
					l->ins_len=0;
					free(l->ins_buf);
					l->ins_buf=0;
					if(*str=='.')
					{
						parse_pseudo_op(str+1);
					}
					else
					{
						ins_write(str);
					}
				}
			}
			else
			{
				pc+=l->ins_len;
				spos+=l->ins_len;
			}
			l=l->next;
			++n;
		}
	}
	while(s);
	l=lines_head;
	spos=0;
	while(l)
	{
		spos+=l->ins_len;
		l=l->next;
	}
	mkelf();
	return 0;
}
