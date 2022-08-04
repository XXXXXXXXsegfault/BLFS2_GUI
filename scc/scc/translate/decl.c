unsigned long int type_size(struct syntax_tree *type,struct syntax_tree *decl);
int is_pointer(struct syntax_tree *decl);
struct syntax_tree *array_function_to_pointer(struct syntax_tree *decl);
void translate_block(struct syntax_tree *root,int push);
struct syntax_tree *get_struct_member_list(struct syntax_tree *type,int def);
char *get_decl_id(struct syntax_tree *decl)
{
	while(strcmp(decl->name,"Identifier"))
	{
		decl=decl->subtrees[0];
	}
	return decl->value;
}
struct syntax_tree *get_decl_type(struct syntax_tree *decl)
{
	if(!strcmp(decl->name,"Identifier"))
	{
		return decl;
	}
	while(strcmp(decl->subtrees[0]->name,"Identifier"))
	{
		decl=decl->subtrees[0];
	}
	return decl;
}
struct syntax_tree *decl_next(struct syntax_tree *decl)
{
	struct syntax_tree *node,*t,*t1,*tp;
	tp=0;
	node=syntax_tree_dup(decl);
	if(!strcmp(decl->name,"Identifier"))
	{
		return node;
	}
	t=get_decl_type(decl);
	t=t->subtrees[0];
	t1=syntax_tree_dup(t);
	t=node;
	while(strcmp(t->subtrees[0]->name,"Identifier"))
	{
		tp=t;
		t=t->subtrees[0];
	}
	syntax_tree_release(t);
	if(tp==0)
	{
		node=t1;
	}
	else
	{
		tp->subtrees[0]=t1;
	}
	return node;
}
void struct_check(struct syntax_tree *type)
{
	int x;
	struct syntax_tree *t,*d;
	struct struct_tab *node;
	x=1;
	while(x<type->count_subtrees)
	{
		t=type->subtrees[x];
		d=get_decl_type(type->subtrees[x+1]);
		if(strcmp(d->name,"pointer"))
		{
			if(!strcmp(t->name,"struct"))
			{
				node=struct_tab_find(t_env.struct_tab,t->subtrees[0]->value);
				if(!node)
				{
					error(t->line,t->col,"incomplete type.");
				}
			}
			if(!strcmp(t->name,"union"))
			{
				node=struct_tab_find(t_env.union_tab,t->subtrees[0]->value);
				if(!node)
				{
					error(t->line,t->col,"incomplete type.");
				}
			}
		}
		x+=2;
	}
}
void decl_check(struct syntax_tree *type,struct syntax_tree *decl)
{
	struct syntax_tree *t;
	int x;
	t=decl;
	while(strcmp(decl->name,"Identifier"))
	{
		if(!strcmp(decl->subtrees[0]->name,"function"))
		{
			if(!strcmp(decl->name,"function"))
			{
				error(decl->line,decl->col,"function returning a function declared.");
			}
			if(!strcmp(decl->name,"array")||!strcmp(decl->name,"array_nosize"))
			{
				error(decl->line,decl->col,"function returning an array declared.");
			}
		}
		if(!strcmp(decl->subtrees[0]->name,"array")||!strcmp(decl->subtrees[0]->name,"array_nosize"))
		{
			if(!strcmp(decl->name,"function"))
			{
				error(decl->line,decl->col,"array of functions declared.");
			}
		}
		if(!strcmp(decl->name,"function"))
		{
			x=1;
			while(x<decl->count_subtrees)
			{
				decl_check(decl->subtrees[x],decl->subtrees[x+1]);
				if(!is_pointer(decl->subtrees[x+1]))
				{
					if(!strcmp(decl->subtrees[x]->name,"struct"))
					{
						error(decl->subtrees[x]->line,decl->subtrees[x]->col,"cannot use structure type as function argument.");
					}
					if(!strcmp(decl->subtrees[x]->name,"union"))
					{
						error(decl->subtrees[x]->line,decl->subtrees[x]->col,"cannot use union type as function argument.");
					}
				}
				x+=2;
			}
		}
		decl=decl->subtrees[0];
	}
	decl=t;
	if(!strcmp(decl->name,"Identifier"))
	{
		if(!strcmp(type->name,"void"))
		{
			error(type->line,type->col,"invalid use of \'void\'.");
		}
	}
	if(!strcmp(decl->name,"function"))
	{
		if(!strcmp(type->name,"struct"))
		{
			error(type->line,type->col,"cannot use structure type as returning value.");
		}
		if(!strcmp(type->name,"union"))
		{
			error(type->line,type->col,"cannot use union type as returning value.");
		}
	}
	if(!strcmp(decl->name,"array")||!strcmp(decl->name,"array_nosize"))
	{
		if(!strcmp(type->name,"void"))
		{
			error(type->line,type->col,"invalid use of \'void\'.");
		}
	}
	if(!strcmp(type->name,"struct")||!strcmp(type->name,"union"))
	{
		x=1;
		while(x<type->count_subtrees)
		{
			decl_check(type->subtrees[x],type->subtrees[x+1]);
			t=get_decl_type(type->subtrees[x+1]);
			if(!strcmp(t->name,"function"))
			{
				error(t->line,t->col,"cannot use function as structure or union member.");
			}
			x+=2;
		}
		if(!strcmp(type->subtrees[0]->value,"<NULL>"))
		{
			if(type->count_subtrees==1)
			{
				error(type->line,type->col,"no structure or union name and no member list.");
			}
		}
		else
		{
			if(type->count_subtrees==1&&!is_pointer(decl)&&!get_struct_member_list(type,0))
			{
				error(type->line,type->col,"incomplete type.");
			}
		}
		struct_check(type);
	}
}
struct syntax_tree *get_struct_member_list(struct syntax_tree *type,int def)
{
	int is_union;
	struct struct_tab *node;
	char *msg;
	if(!strcmp(type->name,"struct"))
	{
		is_union=0;
	}
	else if(!strcmp(type->name,"union"))
	{
		is_union=1;
	}
	else
	{
		return 0;
	}
	if(def&&type->count_subtrees!=1)
	{
		if(strcmp(type->subtrees[0]->value,"<NULL>"))
		{
			if(is_union)
			{
				if(struct_tab_find(t_env.union_tab,type->subtrees[0]->value))
				{
					error(type->line,type->col,"union redefined.");
				}
				struct_tab_add(t_env.union_tab,type->subtrees[0]->value,type);
			}
			else
			{
				if(struct_tab_find(t_env.struct_tab,type->subtrees[0]->value))
				{
					error(type->line,type->col,"structure redefined.");
				}
				struct_tab_add(t_env.struct_tab,type->subtrees[0]->value,type);
			}
		}
		return type;
	}
	if(is_union)
	{
		node=struct_tab_find(t_env.union_tab,type->subtrees[0]->value);
	}
	else
	{
		node=struct_tab_find(t_env.struct_tab,type->subtrees[0]->value);
	}
	if(!node)
	{
		return 0;
	}
	return node->decl;
}
int type_cmp(struct syntax_tree *type1,struct syntax_tree *decl1,struct syntax_tree *type2,struct syntax_tree *decl2)
{
	int x;
	if(strcmp(type1->name,type2->name))
	{
		return 1;
	}
	if(!strcmp(type1->name,"struct")||!strcmp(type1->name,"union"))
	{
		if(strcmp(type1->subtrees[0]->name,type2->subtrees[0]->name))
		{
			return 1;
		}
	}
	while(!strcmp(decl1->name,decl2->name))
	{
		if(!strcmp(decl1->name,"Identifier"))
		{
			return 0;
		}
		if(!strcmp(decl1->name,"function"))
		{
			x=1;
			if(decl1->count_subtrees!=decl2->count_subtrees)
			{
				return 1;
			}
			while(x<decl1->count_subtrees)
			{
				if(type_cmp(decl1->subtrees[x],decl1->subtrees[x+1],decl2->subtrees[x],decl2->subtrees[x+1]))
				{
					return 1;
				}
				x+=2;
			}
		}
		decl1=decl1->subtrees[0];
		decl2=decl2->subtrees[0];
	}
	return 1;
}
void check_decl1(struct syntax_tree *type,struct syntax_tree *decl)
{
	char *name;
	struct id_tab *id;
	struct syntax_tree *decl1;
	name=get_decl_id(decl);
	if(!strcmp(name,"<NULL>"))
	{
		return;
	}
	if(id=id_find2(name))
	{
		if(type_cmp(type,decl,id->type,id->decl))
		{
			error(type->line,type->col,"identifier redeclared as different type.");
		}
	}
}
int check_decl2(struct syntax_tree *type,struct syntax_tree *decl)
{
	char *name;
	struct id_tab *id;
	struct syntax_tree *decl1;
	name=get_decl_id(decl);
	if(!strcmp(name,"<NULL>"))
	{
		return 0;
	}
	if(id=id_find2(name))
	{
		if(id->def)
		{
			error(type->line,type->col,"identifier redefined.");
		}
		if(type_cmp(type,decl,id->type,id->decl))
		{
			error(type->line,type->col,"identifier redeclared as different type.");
		}
		return 1;
	}
	return 0;
}
void add_decl(struct syntax_tree *type,struct syntax_tree *decl,int nodefine,int force_global,struct syntax_tree *init,int no_change_name)
{
	int global,class;
	long int array_size;
	long int size;
	char *name;
	struct syntax_tree *decl1,*decl2;
	struct expr_ret result;
	char *str;
	class=0;
	array_size=-1;
	decl1=get_decl_type(decl);
	if(init)
	{
		error(init->line,init->col,"initializer not supported.");
	}
	if(!strcmp(decl1->name,"function"))
	{
		global=1;
		class=1;
	}
	else
	{
		global=is_global();
		if(!strcmp(decl1->name,"array"))
		{
			class=2;
			calculate_expr(decl1->subtrees[1],&result);
			if(result.is_const==0)
			{
				error(decl->line,decl->col,"cannot determine array size.");
			}
			array_size=result.value;
			expr_ret_release(&result);
		}
		else if(!strcmp(decl1->name,"array_nosize"))
		{
			class=2;
		}
	}
	if(array_size==-1&&init&&!strcmp(init->name,"init"))
	{
		array_size=init->count_subtrees;
	}
	if(global||no_change_name)
	{
		name=xstrdup(get_decl_id(decl));
	}
	else
	{
		name=xstrdup("_$lo");
		name=str_i_app(name,t_env.stack->num);
		name=str_s_app(name,"$");
		name=str_s_app(name,get_decl_id(decl));
	}
	if(nodefine)
	{
		check_decl1(type,decl);
	}
	else
	{
		if(check_decl2(type,decl)&&class!=1)
		{
			free(name);
			return;
		}
	}
	if(class==0)
	{
		if(global||force_global)
		{
			c_write("global ",7);
		}
		else
		{
			c_write("local ",6);
		}
		if(!strcmp(decl1->name,"pointer"))
		{
			c_write("u64 ",4);
		}
		else if(!strcmp(decl1->name,"Identifier"))
		{
			if(!strcmp(type->name,"struct")||!strcmp(type->name,"union"))
			{
				c_write("mem ",4);
				size=type_size(type,decl);
				str=str_i_app(0,size);
				c_write(str,strlen(str));
				free(str);
				c_write(" ",1);
			}
			else
			{
				c_write(type->name,strlen(type->name));
				c_write(" ",1);
			}
		}
	}
	else if(class==1)
	{
		if(!nodefine)
		{
			c_write("fun ",4);
		}
	}
	else if(class==2)
	{
		if(global||force_global)
		{
			c_write("global ",7);
		}
		else
		{
			c_write("local ",6);
		}
		c_write("mem ",4);
		decl2=decl_next(decl);
		size=type_size(type,decl2);
		syntax_tree_release(decl2);
		str=str_i_app(0,size*array_size);
		c_write(str,strlen(str));
		free(str);
		c_write(" ",1);
	}
	if(!nodefine||class!=1)
	{
		c_write(name,strlen(name));
		c_write("\n",1);
	}
	decl1=syntax_tree_dup(decl);
	decl2=get_decl_type(decl1);
	if(!strcmp(decl2->name,"Identifier"))
	{
		free(decl2->value);
		decl2->value=xstrdup(name);
	}
	else
	{
		free(decl2->subtrees[0]->value);
		decl2->subtrees[0]->value=xstrdup(name);
	}
	if(global||force_global)
	{
		id_tab_add(t_env.global_id,name,syntax_tree_dup(type),decl1,!nodefine);
	}
	else
	{
		id_tab_add(t_env.stack->local_id,name,syntax_tree_dup(type),decl1,!nodefine);
		//TODO init
	}
}
void translate_decl(struct syntax_tree *root)
{
	int x;
	struct syntax_tree *type,*decl,*init,*mlist;
	struct syntax_tree *decl1;
	char *name;
	int nodefine;
	x=1;
	nodefine=0;
	type=root->subtrees[0];
	struct_check(type);
	if(mlist=get_struct_member_list(type,1))
	{
		type=mlist;
	}
	while(x<root->count_subtrees)
	{
		decl=root->subtrees[x];
		decl1=get_decl_type(decl);
		if(!strcmp(decl1->name,"function"))
		{
			nodefine=1;
		}
		if(x==root->count_subtrees-1)
		{
			init=0;
		}
		else
		{
			init=root->subtrees[x+1];
			if(strcmp(init->name,"Init"))
			{
				init=0;
			}
		}
		decl_check(type,decl);
		if(init)
		{
			add_decl(type,decl,nodefine,0,init->subtrees[0],0);

		}
		else
		{
			add_decl(type,decl,nodefine,0,0,0);
		}
		++x;
		if(init)
		{
			++x;
		}
	}
}
void translate_fundef(struct syntax_tree *root)
{
	struct syntax_tree *type,*decl,*block;
	struct syntax_tree *decl1,*decl2;
	int x;
	type=root->subtrees[0];
	decl=root->subtrees[1];
	block=root->subtrees[2];
	decl2=get_decl_type(decl);
	if(strcmp(decl2->name,"function"))
	{
		error(decl->line,decl->col,"declaration is not a function.");
	}
	decl_check(type,decl);
	add_decl(type,decl,0,0,0,0);
	c_write("arglist\n",8);
	x=1;
	translate_stack_push();
	while(x<decl2->count_subtrees)
	{
		decl1=array_function_to_pointer(decl2->subtrees[x+1]);
		add_decl(decl2->subtrees[x],decl1,0,0,0,0);
		x+=2;
	}
	c_write("enda\n",5);
	translate_block(block,0);
	translate_stack_pop();
	c_write("endf\n",5);
}
unsigned long int type_size(struct syntax_tree *type,struct syntax_tree *decl)
{
	struct syntax_tree *decl1,*t,*mlist;
	struct expr_ret result;
	unsigned long int ret,size;
	int x;
	decl1=get_decl_type(decl);
	if(!strcmp(decl1->name,"pointer"))
	{
		return 8;
	}
	if(!strcmp(decl1->name,"array"))
	{
		t=decl_next(decl);
		ret=type_size(type,t);
		syntax_tree_release(t);
		calculate_expr(decl1->subtrees[1],&result);
		if(result.is_const==0)
		{
			error(decl->line,decl->col,"cannot determine array size.");
		}
		ret*=result.value;
		expr_ret_release(&result);
		return ret;
	}
	if(!strcmp(decl1->name,"array_nosize"))
	{
		return 0;
	}
	if(!strcmp(decl1->name,"Identifier"))
	{
		if(!strcmp(type->name,"void"))
		{
			return 0;
		}
		if(!strcmp(type->name,"s8")||!strcmp(type->name,"u8"))
		{
			return 1;
		}
		if(!strcmp(type->name,"s16")||!strcmp(type->name,"u16"))
		{
			return 2;
		}
		if(!strcmp(type->name,"s32")||!strcmp(type->name,"u32"))
		{
			return 4;
		}
		if(!strcmp(type->name,"s64")||!strcmp(type->name,"u64"))
		{
			return 8;
		}
		if(!strcmp(type->name,"struct"))
		{
			mlist=get_struct_member_list(type,0);
			ret=0;
			x=1;
			while(x<mlist->count_subtrees)
			{
				ret+=type_size(mlist->subtrees[x],mlist->subtrees[x+1]);
				x+=2;
			}
			return ret;
		}
		if(!strcmp(type->name,"union"))
		{
			mlist=get_struct_member_list(type,0);
			ret=0;
			x=1;
			while(x<mlist->count_subtrees)
			{
				size=type_size(mlist->subtrees[x],mlist->subtrees[x+1]);
				if(size>ret)
				{
					ret=size;
				}
				x+=2;
			}
			return ret;
		}
	}
	return 0;
}
int is_basic_type(struct syntax_tree *type)
{
	if(!strcmp(type->name,"s8")||!strcmp(type->name,"u8"))
	{
		return 1;
	}
	if(!strcmp(type->name,"s16")||!strcmp(type->name,"u16"))
	{
		return 1;
	}
	if(!strcmp(type->name,"s32")||!strcmp(type->name,"u32"))
	{
		return 1;
	}
	if(!strcmp(type->name,"s64")||!strcmp(type->name,"u64"))
	{
		return 1;
	}
	return 0;
}
int is_basic_decl(struct syntax_tree *decl)
{
	struct syntax_tree *decl1;
	decl1=get_decl_type(decl);
	if(!strcmp(decl1->name,"Identifier"))
	{
		return 1;
	}
	return 0;
}
int is_pointer_array_function(struct syntax_tree *decl)
{
	struct syntax_tree *decl1;
	decl1=get_decl_type(decl);
	if(!strcmp(decl1->name,"pointer"))
	{
		return 1;
	}
	if(!strcmp(decl1->name,"array"))
	{
		return 1;
	}
	if(!strcmp(decl1->name,"array_nosize"))
	{
		return 1;
	}
	if(!strcmp(decl1->name,"function"))
	{
		return 1;
	}
	return 0;
}
int is_pointer_array(struct syntax_tree *decl)
{
	struct syntax_tree *decl1;
	decl1=get_decl_type(decl);
	if(!strcmp(decl1->name,"pointer"))
	{
		return 1;
	}
	if(!strcmp(decl1->name,"array"))
	{
		return 1;
	}
	if(!strcmp(decl1->name,"array_nosize"))
	{
		return 1;
	}
	return 0;
}
int is_function(struct syntax_tree *decl)
{
	struct syntax_tree *decl1;
	decl1=get_decl_type(decl);
	if(!strcmp(decl1->name,"function"))
	{
		return 1;
	}
	return 0;
}
int is_array_function(struct syntax_tree *decl)
{
	struct syntax_tree *decl1;
	decl1=get_decl_type(decl);
	if(!strcmp(decl1->name,"function"))
	{
		return 1;
	}
	if(!strcmp(decl1->name,"array"))
	{
		return 1;
	}
	if(!strcmp(decl1->name,"array_nosize"))
	{
		return 1;
	}
	return 0;
}
int is_pointer(struct syntax_tree *decl)
{
	struct syntax_tree *decl1;
	decl1=get_decl_type(decl);
	if(!strcmp(decl1->name,"pointer"))
	{
		return 1;
	}
	return 0;
}
int is_void_ptr(struct syntax_tree *type,struct syntax_tree *decl)
{
	if(strcmp(type->name,"void"))
	{
		return 0;
	}
	if(strcmp(decl->name,"pointer"))
	{
		return 0;
	}
	if(strcmp(decl->subtrees[0]->name,"Identifier"))
	{
		return 0;
	}
	return 1;
}
int is_void(struct syntax_tree *type,struct syntax_tree *decl)
{
	if(strcmp(type->name,"void"))
	{
		return 0;
	}
	if(strcmp(decl->name,"Identifier"))
	{
		return 0;
	}
	return 1;
}
int if_type_compat(struct syntax_tree *type,struct syntax_tree *decl,struct syntax_tree *type2,struct syntax_tree *decl2,int option)
{
	if(is_void(type,decl)||is_void(type2,decl2))
	{
		return 1;
	}
	return 0;
}
struct syntax_tree *array_function_to_pointer(struct syntax_tree *decl)
{
	struct syntax_tree *decl1,*ret,*node;
	ret=syntax_tree_dup(decl);
	decl1=get_decl_type(ret);
	if(!strcmp(decl1->name,"array"))
	{
		decl1->name="pointer";
	}
	else if(!strcmp(decl1->name,"array_nosize"))
	{
		decl1->name="pointer";
	}
	else if(!strcmp(decl1->name,"function"))
	{
		node=mkst("pointer",0,decl1->line,decl1->col);
		st_add_subtree(node,decl1->subtrees[0]);
		decl1->subtrees[0]=node;
	}
	return ret;
}
void array_function_to_pointer2(struct syntax_tree **decl)
{
	struct syntax_tree *decl1;
	decl1=array_function_to_pointer(*decl);
	syntax_tree_release(*decl);
	*decl=decl1;
}
long int get_member_offset(struct syntax_tree *type,char *name)
{
	int x;
	long int off;
	off=0;
	if(!strcmp(type->name,"union"))
	{
		x=1;
		while(x<type->count_subtrees)
		{
			if(!strcmp(get_decl_id(type->subtrees[x+1]),name))
			{
				return 0;
			}
			x+=2;
		}
	}
	else if(!strcmp(type->name,"struct"))
	{
		x=1;
		while(x<type->count_subtrees)
		{
			if(!strcmp(get_decl_id(type->subtrees[x+1]),name))
			{
				return off;
			}
			off+=type_size(type->subtrees[x],type->subtrees[x+1]);
			x+=2;
		}
	}
	return -1;
}
struct syntax_tree *get_member_type(struct syntax_tree *type,char *name)
{
	int x;
	if(!strcmp(type->name,"union"))
	{
		x=1;
		while(x<type->count_subtrees)
		{
			if(!strcmp(get_decl_id(type->subtrees[x+1]),name))
			{
				return type->subtrees[x];
			}
			x+=2;
		}
	}
	else if(!strcmp(type->name,"struct"))
	{
		x=1;
		while(x<type->count_subtrees)
		{
			if(!strcmp(get_decl_id(type->subtrees[x+1]),name))
			{
				return type->subtrees[x];
			}
			x+=2;
		}
	}
	return 0;
}
struct syntax_tree *get_member_decl(struct syntax_tree *type,char *name)
{
	int x;
	if(!strcmp(type->name,"union"))
	{
		x=1;
		while(x<type->count_subtrees)
		{
			if(!strcmp(get_decl_id(type->subtrees[x+1]),name))
			{
				return type->subtrees[x+1];
			}
			x+=2;
		}
	}
	else if(!strcmp(type->name,"struct"))
	{
		x=1;
		while(x<type->count_subtrees)
		{
			if(!strcmp(get_decl_id(type->subtrees[x+1]),name))
			{
				return type->subtrees[x+1];
			}
			x+=2;
		}
	}
	return 0;
}
void translate_static_decl(struct syntax_tree *root)
{
	int x;
	struct syntax_tree *type,*decl,*init,*mlist;
	struct syntax_tree *decl1;
	char *name;
	int nodefine;
	x=1;
	nodefine=0;
	type=root->subtrees[0];
	struct_check(type);
	if(mlist=get_struct_member_list(type,1))
	{
		type=mlist;
	}
	while(x<root->count_subtrees)
	{
		decl=root->subtrees[x];
		decl1=get_decl_type(decl);
		if(!strcmp(decl1->name,"function"))
		{
			nodefine=1;
		}
		if(x==root->count_subtrees-1)
		{
			init=0;
		}
		else
		{
			init=root->subtrees[x+1];
			if(strcmp(init->name,"Init"))
			{
				init=0;
			}
		}
		decl_check(type,decl);
		if(init)
		{
			add_decl(type,decl,nodefine,1,init->subtrees[0],0);

		}
		else
		{
			add_decl(type,decl,nodefine,1,0,0);
		}
		++x;
		if(init)
		{
			++x;
		}
	}
}
void translate_extern_decl(struct syntax_tree *root)
{
	int x;
	struct syntax_tree *type,*decl,*init,*mlist;
	struct syntax_tree *decl1;
	char *name;
	x=1;
	type=root->subtrees[0];
	struct_check(type);
	if(mlist=get_struct_member_list(type,1))
	{
		type=mlist;
	}
	while(x<root->count_subtrees)
	{
		decl=root->subtrees[x];
		decl1=get_decl_type(decl);
		if(x==root->count_subtrees-1)
		{
			init=0;
		}
		else
		{
			init=root->subtrees[x+1];
			if(strcmp(init->name,"Init"))
			{
				init=0;
			}
		}
		decl_check(type,decl);
		if(init)
		{
			add_decl(type,decl,1,1,init->subtrees[0],0);

		}
		else
		{
			add_decl(type,decl,1,1,0,0);
		}
		++x;
		if(init)
		{
			++x;
		}
	}
}
