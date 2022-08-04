void calculate_lsh(struct syntax_tree *root,struct expr_ret *ret)
{
	struct expr_ret left,right;
	struct syntax_tree *decl1;
	struct syntax_tree *new_type,*new_decl;
	char *new_name;
	calculate_expr(root->subtrees[0],&left);
	calculate_expr(root->subtrees[1],&right);
	if(left.is_const&&right.is_const)
	{
		ret->value=left.value<<right.value;
		ret->is_lval=0;
		ret->needs_deref=0;
		ret->is_const=1;
		ret->decl=syntax_tree_dup(left.decl);
		ret->type=syntax_tree_dup(left.type);
		expr_ret_release(&left);
		expr_ret_release(&right);
		return;
	}
	deref_ptr(&left,root->line,root->col);
	deref_ptr(&right,root->line,root->col);
	if(if_type_compat(left.type,left.decl,right.type,right.decl,0))
	{
		error(root->line,root->col,"incompatible type.");
	}
	if(is_pointer_array_function(right.decl))
	{
		error(right.decl->line,right.decl->col,"cannot use pointer as right operand of \'<<\'.");
	}
	if(is_pointer_array_function(left.decl))
	{
		error(right.decl->line,right.decl->col,"cannot use pointer as left operand of \'<<\'.");
	}
	new_name=mktmpname();
	new_decl=syntax_tree_dup(left.decl);
	new_type=syntax_tree_dup(left.type);
	decl1=get_decl_type(new_decl);
	if(!strcmp(decl1->name,"Identifier"))
	{
		free(decl1->value);
		decl1->value=new_name;
	}
	else
	{
		free(decl1->subtrees[0]->value);
		decl1->subtrees[0]->value=new_name;
	}
	ret->is_lval=0;
	ret->needs_deref=0;
	ret->is_const=0;
	ret->decl=new_decl;
	ret->type=new_type;
	add_decl(new_type,new_decl,0,0,0,1);
	c_write("lsh ",4);
	c_write(new_name,strlen(new_name));
	c_write(" ",1);
	if(left.is_const)
	{
		new_name=str_i_app(0,left.value);
	}
	else
	{
		new_name=xstrdup(get_decl_id(left.decl));
	}
	c_write(new_name,strlen(new_name));
	c_write(" ",1);
	free(new_name);
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
