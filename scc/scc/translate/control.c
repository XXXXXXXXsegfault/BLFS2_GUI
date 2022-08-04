void calculate_branch(struct syntax_tree *root,struct expr_ret *ret)
{
	char *new_name;
	struct syntax_tree *type,*decl;
	struct branch_args args;
	control_label_push();
	new_name=mktmpname();
	type=mkst("u64",0,root->line,root->col);
	decl=mkst("Identifier",new_name,root->line,root->col);
	add_decl(type,decl,0,0,0,1);
	args.ltrue=-1;
	args.lfalse=t_env.label->l2;
	c_write("mov ",4);
	c_write(new_name,strlen(new_name));
	c_write(" 0\n",3);

	translate_branch(root,&args);

	c_write("mov ",4);
	c_write(new_name,strlen(new_name));
	c_write(" 1\n",3);
	write_label(t_env.label->l2);
	control_label_pop();
	free(new_name);
	ret->is_const=0;
	ret->is_lval=0;
	ret->needs_deref=0;
	ret->type=type;
	ret->decl=decl;
}
void calculate_relop(struct syntax_tree *root,struct expr_ret *ret,char *ins)
{
	struct expr_ret left,right;
	char *str;
	struct syntax_tree *type,*decl;
	char *new_name;
	calculate_expr(root->subtrees[0],&left);
	calculate_expr(root->subtrees[1],&right);
	deref_ptr(&left,root->line,root->col);
	deref_ptr(&right,root->line,root->col);
	if(left.is_const&&right.is_const)
	{
		ret->value=1;
		if(!strcmp(ins,"bgt "))
		{
			if(left.value>right.value)
			{
				ret->value=0;
			}
		}
		else if(!strcmp(ins,"blt "))
		{
			if(left.value<right.value)
			{
				ret->value=0;
			}
		}
		else if(!strcmp(ins,"bge "))
		{
			if(left.value>=right.value)
			{
				ret->value=0;
			}
		}
		else if(!strcmp(ins,"ble "))
		{
			if(left.value<=right.value)
			{
				ret->value=0;
			}
		}
		else if(!strcmp(ins,"beq "))
		{
			if(left.value==right.value)
			{
				ret->value=0;
			}
		}
		else if(!strcmp(ins,"bne "))
		{
			if(left.value!=right.value)
			{
				ret->value=0;
			}
		}
		ret->is_const=1;
		ret->is_lval=0;
		ret->needs_deref=0;
		ret->type=mkst("u64",0,root->line,root->col);
		ret->decl=mkst("Identifier","<NULL>",root->line,root->col);
		expr_ret_release(&left);
		expr_ret_release(&right);
		return;
	}
	new_name=mktmpname();
	type=mkst("u64",0,root->line,root->col);
	decl=mkst("Identifier",new_name,root->line,root->col);
	add_decl(type,decl,0,0,0,1);
	if(!left.is_const&&!right.is_const)
	{
		if(if_type_compat(left.type,left.decl,right.type,right.decl,0))
		{
			error(root->line,root->col,"incompatible type.");
		}
	}
	control_label_push();

	c_write("mov ",4);
	c_write(new_name,strlen(new_name));
	c_write(" 0\n",3);

	c_write(ins,4);
	if(left.is_const)
	{
		str=str_i_app(0,left.value);
	}
	else
	{
		str=xstrdup(get_decl_id(left.decl));
	}
	c_write(str,strlen(str));
	c_write(" ",1);
	free(str);

	if(right.is_const)
	{
		str=str_i_app(0,right.value);
	}
	else
	{
		str=xstrdup(get_decl_id(right.decl));
	}
	c_write(str,strlen(str));
	c_write(" ",1);
	free(str);

	str=str_i_app(0,t_env.label->l2);
	c_write(str,strlen(str));
	c_write("\n",1);
	free(str);

	c_write("mov ",4);
	c_write(new_name,strlen(new_name));
	c_write(" 1\n",3);

	c_write("label ",6);
	str=str_i_app(0,t_env.label->l2);
	c_write(str,strlen(str));
	c_write("\n",1);
	free(str);

	control_label_pop();
	free(new_name);
	expr_ret_release(&left);
	expr_ret_release(&right);
	ret->type=type;
	ret->decl=decl;
	ret->is_const=0;
	ret->is_lval=0;
	ret->needs_deref=0;
}
void calculate_lnot(struct syntax_tree *root,struct expr_ret *ret)
{
	struct expr_ret result;
	char *str;
	struct syntax_tree *type,*decl;
	char *new_name;
	calculate_expr(root->subtrees[0],&result);
	deref_ptr(&result,root->line,root->col);
	if(result.is_const)
	{
		ret->value=1;
		if(result.value)
		{
			ret->value=0;
		}
		ret->is_const=1;
		ret->is_lval=0;
		ret->needs_deref=0;
		ret->type=mkst("u64",0,root->line,root->col);
		ret->decl=mkst("Identifier","<NULL>",root->line,root->col);
	}
	else
	{
		new_name=mktmpname();
		type=mkst("u64",0,root->line,root->col);
		decl=mkst("Identifier",new_name,root->line,root->col);
		add_decl(type,decl,0,0,0,1);

		control_label_push();
		c_write("mov ",4);
		c_write(new_name,strlen(new_name));
		c_write(" 0\n",3);

		c_write("bne ",4);
		str=xstrdup(get_decl_id(result.decl));
		c_write(str,strlen(str));
		c_write(" 0 ",3);
		free(str);

		str=str_i_app(0,t_env.label->l2);
		c_write(str,strlen(str));
		c_write("\n",1);
		free(str);

		c_write("mov ",4);
		c_write(new_name,strlen(new_name));
		c_write(" 1\n",3);

		c_write("label ",6);
		str=str_i_app(0,t_env.label->l2);
		c_write(str,strlen(str));
		c_write("\n",1);
		free(str);

		ret->type=type;
		ret->decl=decl;
		ret->is_const=0;
		ret->is_lval=0;
		ret->needs_deref=0;

		control_label_pop();
	}
	expr_ret_release(&result);
}
void calculate_land(struct syntax_tree *root,struct expr_ret *ret)
{
	struct expr_ret left,right;
	char *str;
	struct syntax_tree *type,*decl;
	char *new_name;
	calculate_expr(root->subtrees[0],&left);
	deref_ptr(&left,root->line,root->col);
	if(left.is_const)
	{
		if(left.value==0)
		{
			ret->value=0;
			ret->is_const=1;
			ret->is_lval=0;
			ret->needs_deref=0;
			ret->type=mkst("u64",0,root->line,root->col);
			ret->decl=mkst("Identifier","<NULL>",root->line,root->col);
			expr_ret_release(&left);
			return;
		}
	}

	new_name=mktmpname();
	type=mkst("u64",0,root->line,root->col);
	decl=mkst("Identifier",new_name,root->line,root->col);
	add_decl(type,decl,0,0,0,1);

	c_write("mov ",4);
	c_write(new_name,strlen(new_name));
	c_write(" 0\n",3);

	control_label_push();
	
	c_write("beq ",4);
	
	str=get_decl_id(left.decl);
	c_write(str,strlen(str));
	c_write(" 0 ",3);

	str=str_i_app(0,t_env.label->l2);
	c_write(str,strlen(str));
	c_write("\n",1);
	free(str);


	calculate_expr(root->subtrees[1],&right);
	deref_ptr(&right,root->line,root->col);
	if(!right.is_const||right.value!=0)
	{

		if(!right.is_const)
		{
			c_write("beq ",4);
			
			str=get_decl_id(right.decl);
			c_write(str,strlen(str));
			c_write(" 0 ",3);

			str=str_i_app(0,t_env.label->l2);
			c_write(str,strlen(str));
			c_write("\n",1);
			free(str);
		}


		c_write("mov ",4);
		c_write(new_name,strlen(new_name));
		c_write(" 1\n",3);

	}
	c_write("label ",6);
	
	str=str_i_app(0,t_env.label->l2);
	c_write(str,strlen(str));
	c_write("\n",1);
	free(str);

	ret->type=type;
	ret->decl=decl;
	ret->is_const=0;
	ret->is_lval=0;
	ret->needs_deref=0;

	expr_ret_release(&right);
	expr_ret_release(&left);
	control_label_pop();
}
void calculate_lor(struct syntax_tree *root,struct expr_ret *ret)
{
	struct expr_ret left,right;
	char *str;
	struct syntax_tree *type,*decl;
	char *new_name;
	calculate_expr(root->subtrees[0],&left);
	deref_ptr(&left,root->line,root->col);
	if(left.is_const)
	{
		if(left.value!=0)
		{
			ret->value=1;
			ret->is_const=1;
			ret->is_lval=0;
			ret->needs_deref=0;
			ret->type=mkst("u64",0,root->line,root->col);
			ret->decl=mkst("Identifier","<NULL>",root->line,root->col);
			expr_ret_release(&left);
			return;
		}
	}

	new_name=mktmpname();
	type=mkst("u64",0,root->line,root->col);
	decl=mkst("Identifier",new_name,root->line,root->col);
	add_decl(type,decl,0,0,0,1);

	c_write("mov ",4);
	c_write(new_name,strlen(new_name));
	c_write(" 1\n",3);

	control_label_push();
	
	c_write("bne ",4);
	
	str=get_decl_id(left.decl);
	c_write(str,strlen(str));
	c_write(" 0 ",3);

	str=str_i_app(0,t_env.label->l2);
	c_write(str,strlen(str));
	c_write("\n",1);
	free(str);


	calculate_expr(root->subtrees[1],&right);
	deref_ptr(&right,root->line,root->col);
	if(!right.is_const||right.value==0)
	{

		if(!right.is_const)
		{
			c_write("bne ",4);
			
			str=get_decl_id(right.decl);
			c_write(str,strlen(str));
			c_write(" 0 ",3);

			str=str_i_app(0,t_env.label->l2);
			c_write(str,strlen(str));
			c_write("\n",1);
			free(str);
		}


		c_write("mov ",4);
		c_write(new_name,strlen(new_name));
		c_write(" 0\n",3);

	}
	c_write("label ",6);
	
	str=str_i_app(0,t_env.label->l2);
	c_write(str,strlen(str));
	c_write("\n",1);
	free(str);

	ret->type=type;
	ret->decl=decl;
	ret->is_const=0;
	ret->is_lval=0;
	ret->needs_deref=0;

	expr_ret_release(&right);
	expr_ret_release(&left);
	control_label_pop();
}
