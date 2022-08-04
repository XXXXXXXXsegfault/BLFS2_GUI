void calculate_not(struct syntax_tree *root,struct expr_ret *ret)
{
	struct expr_ret result;
	struct syntax_tree *decl1;
	struct syntax_tree *new_type,*new_decl;
	char *new_name;
	calculate_expr(root->subtrees[0],&result);
	array_function_to_pointer2(&result.decl);
	if(result.is_const)
	{
		ret->value=~result.value;
		ret->is_lval=0;
		ret->needs_deref=0;
		ret->is_const=1;
		ret->decl=syntax_tree_dup(result.decl);
		ret->type=syntax_tree_dup(result.type);
		expr_ret_release(&result);
		return;
	}
	deref_ptr(&result,root->line,root->col);
	if(!is_basic_decl(result.decl))
	{
		error(root->line,root->col,"invalid operand for \'~\'");
	}
	new_name=mktmpname();
	new_decl=syntax_tree_dup(result.decl);
	new_type=syntax_tree_dup(result.type);
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
	add_decl(new_type,new_decl,0,0,0,1);
	ret->is_lval=0;
	ret->needs_deref=0;
	ret->is_const=0;
	ret->decl=new_decl;
	ret->type=new_type;
	c_write("not ",4);
	c_write(new_name,strlen(new_name));
	c_write(" ",1);
	new_name=xstrdup(get_decl_id(result.decl));
	c_write(new_name,strlen(new_name));
	c_write("\n",1);
	free(new_name);
	expr_ret_release(&result);
}
