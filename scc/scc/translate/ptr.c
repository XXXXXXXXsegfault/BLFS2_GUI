void calculate_addr(struct syntax_tree *root,struct expr_ret *ret)
{
	struct expr_ret result;
	struct syntax_tree *type,*decl,*decl1;
	char *new_name,*str;
	calculate_expr(root->subtrees[0],&result);
	if(result.is_lval==0)
	{
		error(root->line,root->col,"lvalue required here.");
	}
	if(result.needs_deref)
	{
		ret->is_lval=0;
		ret->needs_deref=0;
		ret->is_const=0;
		ret->type=result.type;
		ret->decl=result.decl;
	}
	else
	{
		new_name=mktmpname();
		decl=get_addr(result.decl);
		type=syntax_tree_dup(result.type);
		decl1=get_decl_type(decl);
		free(decl1->subtrees[0]->value);
		decl1->subtrees[0]->value=new_name;
		add_decl(type,decl,0,0,0,1);
		c_write("adr ",4);
		c_write(new_name,strlen(new_name));
		c_write(" ",1);
		str=get_decl_id(result.decl);
		c_write(str,strlen(str));
		c_write("\n",1);

		ret->is_lval=0;
		ret->needs_deref=0;
		ret->is_const=0;
		ret->type=type;
		ret->decl=decl;
		expr_ret_release(&result);
	}
}
void calculate_deref(struct syntax_tree *root,struct expr_ret *ret)
{
	calculate_expr(root->subtrees[0],ret);
	deref_ptr(ret,root->line,root->col);
	if(!is_pointer_array(ret->decl))
	{
		error(root->line,root->col,"pointer required here.");
	}
	ret->is_lval=1;
	ret->needs_deref=1;
}
void calculate_index(struct syntax_tree *root,struct expr_ret *ret)
{
	struct expr_ret left,right;
	struct syntax_tree *decl1;
	long int scale;
	struct syntax_tree *new_type,*new_decl;
	char *new_name;
	scale=1;

	calculate_expr(root->subtrees[0],&left);
	calculate_expr(root->subtrees[1],&right);

	deref_ptr(&left,root->line,root->col);
	deref_ptr(&right,root->line,root->col);
	if(!is_basic_type(right.type)||!is_basic_decl(right.decl))
	{
		error(root->line,root->col,"array indexes can only be integers.");
	}
	if(!is_pointer_array(left.decl))
	{
		error(root->line,root->col,"pointer required here.");
	}

	new_name=mktmpname();
	decl1=decl_next(left.decl);
	scale=type_size(left.type,decl1);
	syntax_tree_release(decl1);
	if(scale==0)
	{
		scale=1;
	}
	new_decl=array_function_to_pointer(left.decl);
	new_type=syntax_tree_dup(left.type);
	decl1=get_decl_type(new_decl);
	free(decl1->subtrees[0]->value);
	decl1->subtrees[0]->value=new_name;
	if(right.is_const)
	{
		right.value*=scale;
	}
	else
	{
		scale_result(right.type,right.decl,scale);
	}

	ret->is_lval=1;
	ret->needs_deref=1;
	ret->is_const=0;
	ret->decl=new_decl;
	ret->type=new_type;
	add_decl(new_type,new_decl,0,0,0,1);
	c_write("add ",4);
	c_write(new_name,strlen(new_name));
	c_write(" ",1);
	new_name=get_decl_id(left.decl);
	c_write(new_name,strlen(new_name));
	c_write(" ",1);
	if(right.is_const)
	{
		new_name=str_i_app(0,right.value);
	}
	else
	{
		new_name=xstrdup(get_decl_id(right.decl));
	}
	c_write(new_name,strlen(new_name));
	c_write("\n",1);
	free(new_name);
	expr_ret_release(&left);
	expr_ret_release(&right);
}
