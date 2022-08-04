struct l_word_list *p_locate_brackets(struct l_word_list *start,struct l_word_list *end)
{
	int n;
	char *left,*right;
	n=1;
	if(!start)
	{
		return 0;
	}
	if(!strcmp(start->str,"("))
	{
		left="(";
		right=")";
	}
	else if(!strcmp(start->str,"["))
	{
		left="[";
		right="]";
	}
	else if(!strcmp(start->str,"{"))
	{
		left="{";
		right="}";
	}
	else
	{
		return 0;
	}
	start=start->next;
	while(start&&start!=end&&n)
	{
		if(!strcmp(start->str,left))
		{
			++n;
		}
		if(!strcmp(start->str,right))
		{
			--n;
		}
	}
	if(start==end)
	{
		return 0;
	}
	return start;
}
struct syntax_tree
{
	char *name;
	char *value;
	long int line;
	long int col;
	long int count_subtrees;
	struct syntax_tree **subtrees;
};
struct syntax_tree *mkst(char *name,char *value,long int line,long int col)
{
	struct syntax_tree *node;
	node=xmalloc(sizeof(*node));
	if(value)
	{
		node->value=xstrdup(value);
	}
	else
	{
		node->value=0;
	}
	node->name=name;
	node->count_subtrees=0;
	node->subtrees=0;
	node->line=line;
	node->col=col;
	return node;
}
void st_add_subtree(struct syntax_tree *st,struct syntax_tree *subtree)
{
	struct syntax_tree **subtrees;
	subtrees=xmalloc((st->count_subtrees+1)*sizeof(void *));
	memcpy(subtrees,st->subtrees,st->count_subtrees*sizeof(void*));
	subtrees[st->count_subtrees]=subtree;
	++st->count_subtrees;
	free(st->subtrees);
	st->subtrees=subtrees;
}
struct l_word_list *p_current_word;
long int p_current_line;
long int p_current_col;
void parse_next(void)
{
	if(p_current_word==0)
	{
		return;
	}
	p_current_word=p_current_word->next;
	if(p_current_word==0)
	{
		return;
	}
	p_current_line=p_current_word->line;
	p_current_col=p_current_word->col;
}
char *keyw_list[27];
int iskeyw(char *str)
{
	int x;
	x=0;
	while(keyw_list[x])
	{
		if(!strcmp(keyw_list[x],str))
		{
			return 1;
		}
		++x;
	}
	return 0;
}
char *str_list_match(char **str_list,char *str)
{
	int x;
	x=0;
	while(str_list[x])
	{
		if(!strcmp(str_list[x],str))
		{
			return str_list[x];
		}
		++x;
	}
	return 0;
}
int str_list_match2(char **str_list,char *str)
{
	int x;
	x=0;
	while(str_list[x])
	{
		if(!strcmp(str_list[x],str))
		{
			return x;
		}
		++x;
	}
	return -1;
}
char *parse_cstr(void)
{
	if(!p_current_word)
	{
		return "\0";
	}
	return p_current_word->str;
}
#define current p_current_word
#define cstr parse_cstr()
#define line p_current_line
#define col p_current_col
#define next parse_next
#define resume() p_current_word=oldword

#include "expr.c"
#include "type.c"
#include "stmt.c"

struct syntax_tree *parse_file(void)
{
	struct syntax_tree *ret,*node;
	ret=mkst("file",0,line,col);
	while(1)
	{
		if(node=parse_fundef())
		{
			st_add_subtree(ret,node);
		}
		else if(node=parse_asm())
		{
			st_add_subtree(ret,node);
		}
		else
		{
			break;
		}
	}
	return ret;
}

#undef current
#undef cstr
#undef line
#undef col
#undef next
#undef resume
void parse_global_init(void)
{
	p_current_line=1;
	p_current_col=1;
	keyw_list[0]="break";
	keyw_list[1]="case";
	keyw_list[2]="char";
	keyw_list[3]="continue";
	keyw_list[4]="default";
	keyw_list[5]="do";
	keyw_list[6]="else";
	keyw_list[7]="extern";
	keyw_list[8]="for";
	keyw_list[9]="goto";
	keyw_list[10]="if";
	keyw_list[11]="int";
	keyw_list[12]="long";
	keyw_list[13]="return";
	keyw_list[14]="short";
	keyw_list[15]="signed";
	keyw_list[16]="sizeof";
	keyw_list[17]="static";
	keyw_list[18]="switch";
	keyw_list[19]="typedef";
	keyw_list[20]="union";
	keyw_list[21]="unsigned";
	keyw_list[22]="void";
	keyw_list[23]="while";
	keyw_list[24]="asm";
}
