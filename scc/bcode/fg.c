struct ins
{
	char *args[4];
	long int var_num[3];
	long int line;
	short is_const[2];
	char is_global[3];
	char scale;
	unsigned long int const_val[2];
	int count_args;
	int op;
	long int stack_size;
	unsigned long valdef[4];
	unsigned long valuse[4];
	unsigned long valin[4];
	unsigned long valout[4];
	unsigned long int used_regs;
	unsigned int mark;
	unsigned short int mark2;
	unsigned short int mark3;
	long int off;
	long arg_map[11];
	struct ins *branch;
	struct ins *next;
} *ins_head,*ins_end;
long int next_num;
struct label_tab
{
	char *name;
	struct ins *ins;
	struct label_tab *next;
} *label_tab[1021];
void label_tab_add(char *name,struct ins *ins)
{
	int hash;
	struct label_tab *node;
	hash=name_hash(name);
	node=xmalloc(sizeof(*node));
	node->name=xstrdup(name);
	node->ins=ins;
	node->next=label_tab[hash];
	label_tab[hash]=node;
}
struct label_tab *label_tab_find(char *name)
{
	int hash;
	struct label_tab *node;
	hash=name_hash(name);
	node=label_tab[hash];
	while(node)
	{
		if(!strcmp(node->name,name))
		{
			return node;
		}
		node=node->next;
	}
	return 0;
}
struct id_tab
{
	char *name;
	long int num;
	int class;
	int unused;
	long int size;
	long int off;
	long int reg;
	long int storage;
	struct ins *def;
	struct ins *endf;
	struct id_tab *next;
} *global_id[1021],*local_id[1021],*args_id[1021];
void id_tab_add(struct id_tab **tab,char *name,long int num,long int class,long int size,long int off,struct ins *def,struct ins *endf,int unused)
{
	int hash;
	struct id_tab *node;
	hash=name_hash(name);
	node=xmalloc(sizeof(*node));
	node->name=xstrdup(name);
	node->num=num;
	node->class=class;
	node->size=size;
	node->off=off;
	node->def=def;
	node->endf=endf;
	node->reg=-1;
	node->unused=unused;
	node->next=tab[hash];
	tab[hash]=node;
}
struct id_tab *id_tab_find(struct id_tab **tab,char *name)
{
	int hash;
	struct id_tab *node;
	hash=name_hash(name);
	node=tab[hash];
	while(node)
	{
		if(!strcmp(node->name,name))
		{
			return node;
		}
		node=node->next;
	}
	return 0;
}
void id_tab_release(struct id_tab **tab)
{
	int x;
	struct id_tab *node;
	x=0;
	while(x<1021)
	{
		while(tab[x])
		{
			node=tab[x];
			tab[x]=node->next;
			free(node->name);
			free(node);
		}
		++x;
	}
}
struct ins *next_op(struct ins *ins)
{
	while(ins&&!ins->op)
	{
		if(ins->count_args)
		{
			if(!strcmp(ins->args[0],"endf"))
			{
				return 0;
			}
		}
		ins=ins->next;
	}
	return ins;
}

unsigned long int const_to_num(char *str)
{
	unsigned long int ret;
	int x;
	ret=0;
	if(str[0]=='\'')
	{
		++str;
		if(str[0]=='\\')
		{
			if(str[1]=='\\')
			{
				ret='\\';
			}
			else if(str[1]=='n')
			{
				ret='\n';
			}
			else if(str[1]=='t')
			{
				ret='\t';
			}
			else if(str[1]=='v')
			{
				ret='\v';
			}
			else if(str[1]=='r')
			{
				ret='\r';
			}
			else if(str[1]=='\'')
			{
				ret='\'';
			}
			else if(str[1]=='\"')
			{
				ret='\"';
			}
			else if(str[1]=='\?')
			{
				ret='\?';
			}
			else if(str[1]>='0'&&str[1]<='7')
			{
				x=1;
				while(str[x]>='0'&&str[x]<='7')
				{
					ret=(ret<<3)+(str[x]-'0');
					++x;
				}
			}
			else if(str[1]=='x')
			{
				x=2;
				while(1)
				{
					if(str[x]>='0'&&str[x]<='9')
					{
						ret=ret*16+(str[x]-'0');
					}
					else if(str[x]>='A'&&str[x]<='F')
					{
						ret=ret*16+(str[x]-'A'+10);
					}
					else if(str[x]>='a'&&str[x]<='f')
					{
						ret=ret*16+(str[x]-'a'+10);
					}
					else
					{
						break;
					}
					++x;
				}
			}
			else
			{
				ret='\\';
			}
		}
		else
		{
			ret=str[0];
		}
	}
	else if(str[0]>='1'&&str[0]<='9')
	{
		x=0;
		while(str[x]>='0'&&str[x]<='9')
		{
			ret=ret*10+(str[x]-'0');
			++x;
		}
	}
	else if(str[1]=='X'||str[1]=='x')
	{
		x=2;
		while(1)
		{
			if(str[x]>='0'&&str[x]<='9')
			{
				ret=ret*16+(str[x]-'0');
			}
			else if(str[x]>='A'&&str[x]<='F')
			{
				ret=ret*16+(str[x]-'A'+10);
			}
			else if(str[x]>='a'&&str[x]<='f')
			{
				ret=ret*16+(str[x]-'a'+10);
			}
			else
			{
				break;
			}
			++x;
		}
	}
	else
	{
		x=0;
		while(str[x]>='0'&&str[x]<='7')
		{
			ret=(ret<<3)+(str[x]-'0');
			++x;
		}
	}
	return ret;
}

void ins_add(char *str)
{
	int x;
	struct ins *node;
	node=xmalloc(sizeof(*node));
	node->line=current_line;
	node->count_args=0;
	node->op=0;
	node->used_regs=0;
	node->is_const[0]=0;
	node->is_const[1]=0;
	node->is_global[0]=0;
	node->is_global[1]=0;
	node->is_global[2]=0;
	node->const_val[0]=0;
	node->const_val[1]=0;
	memset(node->arg_map,0xff,11*8);
	x=0;
	while(x<4)
	{
		if(node->args[x]=sdup(str))
		{
			++node->count_args;
		}
		str=snext(str);
		++x;
		if(x==1&&node->count_args)
		{
			if(!strcmp(node->args[0],"mov"))
			{
				node->op=12;
			}
			if(!strcmp(node->args[0],"not"))
			{
				node->op=1;
			}
			if(!strcmp(node->args[0],"neg"))
			{
				node->op=1;
			}
			if(!strcmp(node->args[0],"adr"))
			{
				node->op=11;
			}
			if(!strcmp(node->args[0],"add"))
			{
				node->op=13;
			}
			if(!strcmp(node->args[0],"sub"))
			{
				node->op=14;
			}
			if(!strcmp(node->args[0],"mul"))
			{
				node->op=17;
			}
			if(!strcmp(node->args[0],"div"))
			{
				node->op=2;
			}
			if(!strcmp(node->args[0],"mod"))
			{
				node->op=2;
			}
			if(!strcmp(node->args[0],"and"))
			{
				node->op=2;
			}
			if(!strcmp(node->args[0],"orr"))
			{
				node->op=2;
			}
			if(!strcmp(node->args[0],"eor"))
			{
				node->op=2;
			}
			if(!strcmp(node->args[0],"lsh"))
			{
				node->op=2;
			}
			if(!strcmp(node->args[0],"rsh"))
			{
				node->op=2;
			}
			if(!strcmp(node->args[0],"push"))
			{
				node->op=3;
			}
			if(!strcmp(node->args[0],"retval"))
			{
				node->op=9;
			}
			if(!strcmp(node->args[0],"ret"))
			{
				node->op=10;
			}
			if(!strcmp(node->args[0],"call"))
			{
				node->op=4;
			}
			if(!strcmp(node->args[0],"stb"))
			{
				node->op=5;
			}
			if(!strcmp(node->args[0],"stw"))
			{
				node->op=5;
			}
			if(!strcmp(node->args[0],"stl"))
			{
				node->op=5;
			}
			if(!strcmp(node->args[0],"stq"))
			{
				node->op=5;
			}
			if(!strcmp(node->args[0],"ldb"))
			{
				node->op=6;
			}
			if(!strcmp(node->args[0],"ldw"))
			{
				node->op=6;
			}
			if(!strcmp(node->args[0],"ldl"))
			{
				node->op=6;
			}
			if(!strcmp(node->args[0],"ldq"))
			{
				node->op=6;
			}
			if(!strcmp(node->args[0],"beq"))
			{
				node->op=7;
			}
			if(!strcmp(node->args[0],"bne"))
			{
				node->op=7;
			}
			if(!strcmp(node->args[0],"bgt"))
			{
				node->op=7;
			}
			if(!strcmp(node->args[0],"blt"))
			{
				node->op=7;
			}
			if(!strcmp(node->args[0],"bge"))
			{
				node->op=7;
			}
			if(!strcmp(node->args[0],"ble"))
			{
				node->op=7;
			}
			if(!strcmp(node->args[0],"bal"))
			{
				node->op=8;
			}
		}
	}
	node->next=0;
	if(ins_head)
	{
		ins_end->next=node;
	}
	else
	{
		ins_head=node;
	}
	ins_end=node;
}
long int data_size;
void load_global_vars(void)
{
	struct ins *node;
	long int class,size;
	char *name;
	long int off;
	off=0;
	node=ins_head;
	while(node)
	{
		++next_num;
		if(node->count_args>=3)
		{
			if(!strcmp(node->args[0],"global"))
			{
				if(!strcmp(node->args[1],"mem"))
				{
					if(node->count_args<4)
					{
						error(node->line,"too few arguments.");
					}
					size=const_to_num(node->args[2]);
					class=0;
					name=node->args[3];
				}
				else if(!strcmp(node->args[1],"s8"))
				{
					size=1;
					class=1;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"u8"))
				{
					size=1;
					class=2;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"s16"))
				{
					size=2;
					class=3;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"u16"))
				{
					size=2;
					class=4;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"s32"))
				{
					size=4;
					class=5;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"u32"))
				{
					size=4;
					class=6;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"s64"))
				{
					size=8;
					class=7;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"u64"))
				{
					size=8;
					class=8;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"void"))
				{
					size=8;
					class=8;
					name=node->args[2];
				}
				else
				{
					error(node->line,"invalid type.");
				}
				if(!id_tab_find(global_id,name))
				{
					if(strcmp(name,"<NULL>"))
					{
						id_tab_add(global_id,name,next_num,class,size,off,0,0,0);
					}
					else
					{
						size=0;
					}
				}
			}
			else
			{
				size=0;
			}
		}
		else
		{
			size=0;
		}
		off+=size;
		off=off+15&0xfffffffffffffff0;
		node=node->next;
	}
	data_size=off;
}
void load_labels(void)
{
	struct ins *node;
	node=ins_head;
	while(node)
	{
		if(node->count_args>=2)
		{
			if(!strcmp(node->args[0],"label"))
			{
				if(!label_tab_find(node->args[1]))
				{
					label_tab_add(node->args[1],node);
				}
			}
		}
		node=node->next;
	}
}
void load_branches(void)
{
	struct ins *node;
	int b;
	struct label_tab *label;
	node=ins_head;
	while(node)
	{
		if(node->count_args>1)
		{
			b=0;
			if(!strcmp(node->args[0],"beq"))
			{
				b=1;
			}
			else if(!strcmp(node->args[0],"bne"))
			{
				b=1;
			}
			else if(!strcmp(node->args[0],"blt"))
			{
				b=1;
			}
			else if(!strcmp(node->args[0],"bgt"))
			{
				b=1;
			}
			else if(!strcmp(node->args[0],"ble"))
			{
				b=1;
			}
			else if(!strcmp(node->args[0],"bge"))
			{
				b=1;
			}
			else if(!strcmp(node->args[0],"bal"))
			{
				b=2;
			}
			if(b==1)
			{
				if(node->count_args<4)
				{
					error(node->line,"too few arguments.");
				}
				label=label_tab_find(node->args[3]);
				if(!label)
				{
					error(node->line,"label not defined.");
				}
				node->branch=label->ins;
			}
			else if(b==2)
			{
				label=label_tab_find(node->args[1]);
				if(!label)
				{
					error(node->line,"label not defined.");
				}
				node->branch=label->ins;
			}
			else
			{
				node->branch=0;
			}
		}
		else
		{
			node->branch=0;
		}
		node=node->next;
	}
}
struct ins *fstart,*fend;
long int fline;
long int fend_line;
struct id_list
{
	long int num;
	char *name;
	long int flag;
	struct id_list *next;
} *id_list_head;
void id_list_add(char *name)
{
	struct id_list *node;
	struct id_tab *id;
	node=xmalloc(sizeof(*node));
	node->flag=0;
	id=id_tab_find(local_id,name);
	if(!id)
	{
		node->flag=1;
		id=id_tab_find(args_id,name);
		if(!id)
		{
			id=id_tab_find(global_id,name);
		}
	}
	if(id->unused)
	{
		free(node);
		return;
	}
	node->num=id->num;
	node->name=xstrdup(name);
	node->next=id_list_head;
	id_list_head=node;
}
void id_list_release(void)
{
	struct id_list *node;
	while(node=id_list_head)
	{
		id_list_head=node->next;
		free(node->name);
		free(node);
	}
}
#include "reg.c"
//#include "optimize.c"
void load_local_vars(void)
{
	struct ins *node;
	long int class,size,unused;
	char *name;
	long int arglist,off;
	int count;
	int x;
	node=ins_head;
	arglist=0;
	x=0;
	while(node)
	{
		++next_num;
		if(node->count_args&&!strcmp(node->args[0],"arglist"))
		{
			arglist=1;
			off=0;
		}
		else if(node->count_args&&!strcmp(node->args[0],"enda"))
		{
			arglist=0;
		}
		else if(node->count_args>=3)
		{
			if(!strcmp(node->args[0],"local"))
			{
				unused=0;
				if(!strcmp(node->args[1],"mem"))
				{
					if(node->count_args<4)
					{
						error(node->line,"too few arguments.");
					}
					size=const_to_num(node->args[2]);
					class=0;
					name=node->args[3];
				}
				else if(!strcmp(node->args[1],"s8"))
				{
					size=1;
					class=1;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"u8"))
				{
					size=1;
					class=2;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"s16"))
				{
					size=2;
					class=3;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"u16"))
				{
					size=2;
					class=4;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"s32"))
				{
					size=4;
					class=5;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"u32"))
				{
					size=4;
					class=6;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"s64"))
				{
					size=8;
					class=7;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"u64"))
				{
					size=8;
					class=8;
					name=node->args[2];
				}
				else if(!strcmp(node->args[1],"void"))
				{
					size=8;
					class=8;
					unused=1;
					name=node->args[2];
				}
				else
				{
					error(node->line,"invalid type.");
				}
				if(!id_tab_find(global_id,name))
				{
					if(arglist)
					{
						id_tab_add(args_id,name,next_num,class,size,off,0,0,0);
						++off;
					}
					else
					{
						id_tab_add(local_id,name,next_num,class,size,0,0,0,unused);
					}
				}
				else
				{
					error(node->line,"duplicate identifier.");
				}
			}
		}
		node=node->next;
	}
}
void reg_init(void)
{
	struct ins *ins,*node;
	struct id_tab *id;
	int x,class;
	long int arglist;
	char *name;
	long int num;
	long int off,size;
	struct id_list *il;
	char c;
	ins=ins_head;
	while(ins)
	{
		x=1;
		while(x<ins->count_args)
		{
			if(id=id_tab_find(local_id,ins->args[x]))
			{
				ins->var_num[x-1]=id->num;
			}
			else if(id=id_tab_find(args_id,ins->args[x]))
			{
				ins->var_num[x-1]=id->num;
			}
			else if(id=id_tab_find(global_id,ins->args[x]))
			{
				ins->var_num[x-1]=id->num;
				ins->is_global[x-1]=1;
			}
			else
			{
				ins->var_num[x-1]=0;
				if(x>=2)
				{
					c=ins->args[x][0];
					if(c>='0'&&c<='9'||c=='\'')
					{
						ins->is_const[x-2]=1;
						ins->const_val[x-2]=const_to_num(ins->args[x]);
					}
				}
			}
			++x;
		}
		while(x<4)
		{
			ins->var_num[x-1]=0;
			++x;
		}
		ins=ins->next;
	}
	//do_optimize();
	ins=ins_head;
	fstart=ins;
	fend=ins;
	x=0;
	arglist=0;
	fline=0;
	fend_line=0;
	num=0;
	off=0;
	while(ins)
	{
		if(ins->count_args)
		{
			if(!strcmp(ins->args[0],"fun"))
			{
				fstart=ins;
				fend=ins;
				fline=num;
				fend_line=num;
				off=0;
				while(fend&&(!fend->count_args||strcmp(fend->args[0],"endf")))
				{
					fend=fend->next;
					++fend_line;
				}
			}
			else if(!strcmp(ins->args[0],"arglist"))
			{
				arglist=1;
			}
			else if(!strcmp(ins->args[0],"enda"))
			{
				arglist=0;
			}
			else if(!strcmp(ins->args[0],"endf"))
			{
				if(fstart)
				{
					if(x)
					{
						x=0;
						get_reg();
					}
					reg_map_release();
				}
			}
			else if(!strcmp(ins->args[0],"local"))
			{
				if(!strcmp(ins->args[1],"mem"))
				{
					if(ins->count_args<4)
					{
						error(ins->line,"too few arguments.");
					}
					class=0;
					name=ins->args[3];
				}
				else if(!strcmp(ins->args[1],"s8"))
				{
					class=1;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"u8"))
				{
					class=2;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"s16"))
				{
					class=3;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"u16"))
				{
					class=4;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"s32"))
				{
					class=5;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"u32"))
				{
					class=6;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"s64"))
				{
					class=7;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"u64"))
				{
					class=8;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"void"))
				{
					class=8;
					name=ins->args[2];
				}
				else
				{
					error(ins->line,"invalid type.");
				}
				if(class!=0&&fstart)
				{
					id_list_add(name);
					++x;
					if(x==256)
					{
						x=0;
						get_reg();
					}
				}
			}
		}
		++num;
		ins=ins->next;
	}
	off=0;
	arglist=0;
	ins=ins_head;
	fstart=0;
	x=0;
	while(ins)
	{
		if(ins->count_args)
		{
			if(!strcmp(ins->args[0],"fun"))
			{
				fstart=ins;
				x=0;
				off=0;
				fstart->stack_size=0;
			}
			else if(!strcmp(ins->args[0],"arglist"))
			{
				arglist=1;
			}
			else if(!strcmp(ins->args[0],"enda"))
			{
				arglist=0;
			}
			else if(!strcmp(ins->args[0],"endf"))
			{
				if(fstart)
				{
					fstart->stack_size-=32;
					if(x>=11)
					{
						fstart->stack_size-=(x-10)*8;
					}
					fstart->stack_size&=0xfffffffffffffff0;
				}
			}
			else if(!strcmp(ins->args[0],"adr"))
			{
				if(ins->count_args<3)
				{
					error(ins->line,"too few arguments.");
				}
				if(id=id_tab_find(local_id,ins->args[2]))
				{
					if(id->reg>=0)
					{
						id->reg=-1;
						off-=size;
						off=off&0xfffffffffffffff0;
						id->off=off;
						if(fstart)
						{
							fstart->stack_size=off;
						}
					}
				}
				else if(id=id_tab_find(args_id,ins->args[2]))
				{
					if(id->reg>=0)
					{
						id->reg=-1;
					}
				}
			}
			else if(!strcmp(ins->args[0],"local"))
			{
				if(ins->count_args<3)
				{
					error(ins->line,"too few arguments.");
				}
				if(!strcmp(ins->args[1],"mem"))
				{
					if(ins->count_args<4)
					{
						error(ins->line,"too few arguments.");
					}
					size=const_to_num(ins->args[2]);
					name=ins->args[3];
				}
				else if(!strcmp(ins->args[1],"s8"))
				{
					size=1;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"u8"))
				{
					size=1;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"s16"))
				{
					size=2;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"u16"))
				{
					size=2;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"s32"))
				{
					size=4;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"u32"))
				{
					size=4;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"s64"))
				{
					size=8;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"u64"))
				{
					size=8;
					name=ins->args[2];
				}
				else if(!strcmp(ins->args[1],"void"))
				{
					size=8;
					name=ins->args[2];
				}
				else
				{
					error(ins->line,"invalid type.");
				}

				if(id=id_tab_find(local_id,name))
				{
					if(id->reg==-1)
					{
						off-=size;
						off=off&0xfffffffffffffff0;
						id->off=off;
						if(fstart)
						{
							fstart->stack_size=off;
						}
					}
					else if(id->reg>x)
					{
						x=id->reg;
					}
				}
			}
		}
		++num;
		ins=ins->next;
	}
	
}
struct id_tab *id_find(char *name)
{
	struct id_tab *ret;
	if(ret=id_tab_find(local_id,name))
	{
		ret->storage=0;
		return ret;
	}
	if(ret=id_tab_find(args_id,name))
	{
		ret->storage=1;
		return ret;
	}
	if(ret=id_tab_find(global_id,name))
	{
		ret->storage=2;
		return ret;
	}
	return 0;
}
