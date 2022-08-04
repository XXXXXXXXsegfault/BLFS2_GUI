void translate_stmt(struct syntax_tree *root);
void translate_block(struct syntax_tree *root,int push)
{
	int x;
	struct syntax_tree *node;
	x=0;
	if(push)
	{
		translate_stack_push();
	}
	while(x<root->count_subtrees)
	{
		node=root->subtrees[x];
		translate_stmt(node);
		++x;
	}
	if(push)
	{
		translate_stack_pop();
	}
}
void translate_return(struct syntax_tree *root)
{
	struct expr_ret ret;
	char *name;
	if(root->count_subtrees)
	{
		calculate_expr(root->subtrees[0],&ret);
		deref_ptr(&ret,root->line,root->col);
		if(ret.is_const)
		{
			name=str_i_app(0,ret.value);
		}
		else
		{
			name=xstrdup(get_decl_id(ret.decl));
		}
		c_write("retval ",7);
		c_write(name,strlen(name));
		expr_ret_release(&ret);
		free(name);
		c_write("\n",1);
	}
	else
	{
		c_write("ret\n",4);
	}
}
void translate_if(struct syntax_tree *root)
{
	struct branch_args args;
	control_label_push();

	args.ltrue=-1;
	args.lfalse=t_env.label->l3;
	translate_branch(root->subtrees[0],&args);
	translate_stmt(root->subtrees[1]);
	write_label(t_env.label->l3);

	control_label_pop();
}
void translate_while(struct syntax_tree *root)
{
	int l;
	struct control_labels *label;
	struct branch_args args;
	l=t_env.label_in_use;
	control_label_push();
	t_env.label_in_use=1;
	label=t_env.break_label;
	t_env.break_label=t_env.label;

	args.ltrue=-1;
	args.lfalse=t_env.label->l3;
	translate_branch(root->subtrees[0],&args);
	write_label(t_env.label->l1);
	translate_stmt(root->subtrees[1]);
	args.ltrue=t_env.label->l1;
	args.lfalse=-1;
	translate_branch(root->subtrees[0],&args);
	write_label(t_env.label->l3);

	control_label_pop();
	t_env.label_in_use=l;
	t_env.break_label=label;
}
void translate_dowhile(struct syntax_tree *root)
{
	int l;
	struct control_labels *label;
	struct branch_args args;
	l=t_env.label_in_use;
	control_label_push();
	t_env.label_in_use=1;
	label=t_env.break_label;
	t_env.break_label=t_env.label;

	write_label(t_env.label->l1);
	translate_stmt(root->subtrees[0]);
	args.ltrue=t_env.label->l1;
	args.lfalse=-1;
	translate_branch(root->subtrees[1],&args);
	write_label(t_env.label->l3);

	control_label_pop();
	t_env.label_in_use=l;
	t_env.break_label=label;
}
void translate_break(struct syntax_tree *root)
{
	char *name;
	if(!t_env.label_in_use)
	{
		error(root->line,root->col,"unexpected \'break\'.");
	}
	c_write("bal ",4);
	name=str_i_app(0,t_env.break_label->l3);
	c_write(name,strlen(name));
	c_write("\n",1);
	free(name);

}
void translate_ifelse(struct syntax_tree *root)
{
	struct branch_args args;
	control_label_push();

	args.ltrue=-1;
	args.lfalse=t_env.label->l2;
	translate_branch(root->subtrees[0],&args);
	translate_stmt(root->subtrees[1]);
	c_write("bal ",4);
	write_label_name(t_env.label->l3);
	c_write("\n",1);
	write_label(t_env.label->l2);
	translate_stmt(root->subtrees[2]);
	write_label(t_env.label->l3);

	control_label_pop();
}
void translate_asm(struct syntax_tree *node)
{
	char *str;
	c_write("asm ",4);
	str=node->subtrees[0]->value;
	c_write(str,strlen(str));
	c_write("\n",1);
}
void translate_stmt(struct syntax_tree *node)
{
	struct expr_ret ret;
	if(!strcmp(node->name,"asm"))
	{
		translate_asm(node);
	}
	if(!strcmp(node->name,"decl"))
	{
		translate_decl(node);
	}
	if(!strcmp(node->name,"static_decl"))
	{
		translate_static_decl(node);
	}
	if(!strcmp(node->name,"extern_decl"))
	{
		translate_extern_decl(node);
	}
	else if(!strcmp(node->name,"expr"))
	{
		calculate_expr(node->subtrees[0],&ret);
		expr_ret_release(&ret);
	}
	else if(!strcmp(node->name,"block"))
	{
		translate_block(node,1);
	}
	else if(!strcmp(node->name,"return"))
	{
		translate_return(node);
	}
	else if(!strcmp(node->name,"if"))
	{
		translate_if(node);
	}
	else if(!strcmp(node->name,"ifelse"))
	{
		translate_ifelse(node);
	}
	else if(!strcmp(node->name,"while"))
	{
		translate_while(node);
	}
	else if(!strcmp(node->name,"dowhile"))
	{
		translate_dowhile(node);
	}
	else if(!strcmp(node->name,"break"))
	{
		translate_break(node);
	}

}
