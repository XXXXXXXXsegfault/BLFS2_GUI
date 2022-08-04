struct syntax_tree *syntax_tree_dup(struct syntax_tree *root)
{
	struct syntax_tree *node;
	int x;
	x=0;
	if(root==0)
	{
		return 0;
	}
	node=xmalloc(sizeof(*node));
	memcpy(node,root,sizeof(*node));
	if(node->value)
	{
		node->value=xstrdup(root->value);
	}
	if(node->count_subtrees)
	{
		node->subtrees=xmalloc(node->count_subtrees*sizeof(void *));
		while(x<node->count_subtrees)
		{
			node->subtrees[x]=syntax_tree_dup(root->subtrees[x]);
			++x;
		}
	}
	return node;
}
void syntax_tree_release(struct syntax_tree *root)
{
	int x;
	x=0;
	if(root==0)
	{
		return;
	}
	while(x<root->count_subtrees)
	{
		syntax_tree_release(root->subtrees[x]);
		++x;
	}
	free(root->value);
	free(root->subtrees);
	free(root);
}
struct id_tab
{
	char *name;
	struct syntax_tree *type;
	struct syntax_tree *decl;
	struct id_tab *next;
	long int def;
};
struct struct_tab
{
	char *name;
	struct syntax_tree *decl;
	struct struct_tab *next;
};
struct translate_stack
{
	struct id_tab *local_id[1021];
	long int num;
	struct translate_stack *next;
};
struct control_labels
{
	long int l1;
	long int l2;
	long int l3;
	struct control_labels *next;
};
struct translate_env
{
	long int next_num;
	long int next_label;
	struct id_tab *global_id[1021];
	struct struct_tab *struct_tab[1021];
	struct struct_tab *union_tab[1021];
	struct translate_stack *stack;
	int write;
	int label_in_use;
	struct control_labels *label;
	struct control_labels *break_label;
} t_env;
struct expr_ret
{
	unsigned long int value;
	int is_lval;
	short int is_const;
	short int needs_deref;
	struct syntax_tree *type;
	struct syntax_tree *decl;
};
void expr_ret_release(struct expr_ret *ret)
{
	syntax_tree_release(ret->type);
	syntax_tree_release(ret->decl);
}
void calculate_expr(struct syntax_tree *root,struct expr_ret *ret);
struct struct_tab *struct_tab_find(struct struct_tab **tab,char *name)
{
	int hash;
	struct struct_tab *node;
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
void struct_tab_add(struct struct_tab **tab,char *name,struct syntax_tree *decl)
{
	int hash;
	struct struct_tab *node;
	hash=name_hash(name);
	node=xmalloc(sizeof(node));
	node->name=name;
	node->decl=decl;
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
void id_tab_add(struct id_tab **tab,char *name,struct syntax_tree *type,struct syntax_tree *decl,int def)
{
	int hash;
	struct id_tab *node;
	hash=name_hash(name);
	node=xmalloc(sizeof(*node));
	node->name=name;
	node->type=type;
	node->decl=decl;
	node->def=def;
	node->next=tab[hash];
	tab[hash]=node;
}
struct id_tab *id_find(char *name)
{
	char *name1;
	struct translate_stack *node;
	struct id_tab *ret;
	node=t_env.stack;
	while(node)
	{
		name1=xstrdup("_$lo");
		name1=str_i_app(name1,node->num);
		name1=str_s_app(name1,"$");
		name1=str_s_app(name1,name);
		if(ret=id_tab_find(node->local_id,name1))
		{
			free(name1);
			return ret;
		}
		if(ret=id_tab_find(t_env.global_id,name1))
		{
			free(name1);
			return ret;
		}
		free(name1);
		node=node->next;
	}
	return id_tab_find(t_env.global_id,name);
}
struct id_tab *id_find2(char *name)
{
	char *name1;
	struct translate_stack *node;
	struct id_tab *ret;
	node=t_env.stack;
	if(node)
	{
		name1=xstrdup("_$lo");
		name1=str_i_app(name1,node->num);
		name1=str_s_app(name1,"$");
		name1=str_s_app(name1,name);
		if(ret=id_tab_find(node->local_id,name1))
		{
			free(name1);
			return ret;
		}
		if(ret=id_tab_find(t_env.global_id,name1))
		{
			free(name1);
			return ret;
		}
		free(name1);
		return 0;
	}
	return id_tab_find(t_env.global_id,name);
}
void translate_stack_push(void)
{
	struct translate_stack *node;
	node=xmalloc(sizeof(*node));
	memset(node,0,sizeof(*node));
	node->num=t_env.next_num;
	++t_env.next_num;
	node->next=t_env.stack;
	t_env.stack=node;
}
void translate_stack_pop(void)
{
	struct translate_stack *node;
	struct id_tab *tab,*t;
	int x;
	x=0;
	node=t_env.stack;
	while(x<1021)
	{
		tab=node->local_id[x];
		while(tab)
		{
			t=tab;
			tab=t->next;
			syntax_tree_release(t->type);
			syntax_tree_release(t->decl);
			free(t->name);
			free(t);
		}
		++x;
	}

	t_env.stack=node->next;
	free(node);
}
void control_label_push(void)
{
	struct control_labels *node;
	node=xmalloc(sizeof(*node));
	t_env.next_label+=3;
	node->l1=t_env.next_label;
	node->l2=t_env.next_label+1;
	node->l3=t_env.next_label+2;
	node->next=t_env.label;
	t_env.label=node;
}
void control_label_pop(void)
{
	struct control_labels *node;
	node=t_env.label;
	t_env.label=node->next;
	free(node);
}
int is_global(void)
{
	if(t_env.stack==0)
	{
		return 1;
	}
	return 0;
}
char outc_buf[65536];
int outc_x;
void outc(char c)
{
	int n;
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
	if(t_env.write)
	{
		return;
	}
	while(size)
	{
		outc(*buf);
		++buf;
		--size;
	}
}
#include "decl.c"
#include "expr.c"
#include "stmt.c"
void translate_file(struct syntax_tree *root)
{
	int x;
	x=0;
	while(x<root->count_subtrees)
	{
		if(!strcmp(root->subtrees[x]->name,"decl"))
		{
			translate_decl(root->subtrees[x]);
		}
		else if(!strcmp(root->subtrees[x]->name,"fundef"))
		{
			translate_fundef(root->subtrees[x]);
		}
		else if(!strcmp(root->subtrees[x]->name,"asm"))
		{
			translate_asm(root->subtrees[x]);
		}
		++x;
	}
}
