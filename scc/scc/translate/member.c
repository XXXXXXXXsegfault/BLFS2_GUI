void calculate_member(struct syntax_tree *root,struct expr_ret *ret)
{
	struct syntax_tree *mlist,*type,*decl,*decl1;
	long int off;
	struct expr_ret result;
	char *new_name;

	calculate_expr(root->subtrees[0],&result);
	if(result.needs_deref)
	{
		decl1=decl_next(result.decl);
		if(!is_basic_decl(decl1))
		{
			error(root->line,root->col,"bad member name.");
		}
		syntax_tree_release(decl1);
	}
	else
	{
		if(!is_basic_decl(result.decl))
		{
			error(root->line,root->col,"bad member name.");
		}
	}
	mlist=get_struct_member_list(result.type,0);
	if(!mlist)
	{
		error(root->line,root->col,"bad member name.");
	}
	type=get_member_type(mlist,root->subtrees[1]->value);
	if(type==0)
	{
		error(root->line,root->col,"bad member name.");
	}
	decl=get_member_decl(mlist,root->subtrees[1]->value);
	off=get_member_offset(mlist,root->subtrees[1]->value);

	type=syntax_tree_dup(type);
	decl=get_addr(decl);

	new_name=mktmpname();
	decl1=get_decl_type(decl);
	free(decl1->subtrees[0]->value);
	decl1->subtrees[0]->value=new_name;

	add_decl(type,decl,0,0,0,1);

	c_write("add ",4);
	c_write(new_name,strlen(new_name));
	c_write(" ",1);
	new_name=get_decl_id(result.decl);
	c_write(new_name,strlen(new_name));
	c_write(" ",1);
	new_name=str_i_app(0,off);
	c_write(new_name,strlen(new_name));
	c_write("\n",1);
	free(new_name);

	ret->is_const=0;
	ret->is_lval=1;
	ret->needs_deref=1;
	ret->type=type;
	ret->decl=decl;
	expr_ret_release(&result);
}
void calculate_member_ptr(struct syntax_tree *root,struct expr_ret *ret)
{
	struct syntax_tree *mlist,*type,*decl,*decl1;
	long int off;
	struct expr_ret result;
	char *new_name;

	calculate_expr(root->subtrees[0],&result);
	deref_ptr(&result,root->line,root->col);
	if(!is_pointer_array(result.decl))
	{
		error(root->line,root->col,"invalid use of \'->\'.");
	}
	decl1=decl_next(result.decl);
	if(!is_basic_decl(decl1))
	{
		error(root->line,root->col,"invalid use of \'->\'.");
	}
	syntax_tree_release(decl1);

	mlist=get_struct_member_list(result.type,0);
	if(!mlist)
	{
		error(root->line,root->col,"bad member name.");
	}
	type=get_member_type(mlist,root->subtrees[1]->value);
	if(type==0)
	{
		error(root->line,root->col,"bad member name.");
	}
	decl=get_member_decl(mlist,root->subtrees[1]->value);
	off=get_member_offset(mlist,root->subtrees[1]->value);
	type=syntax_tree_dup(type);
	decl=get_addr(decl);

	new_name=mktmpname();
	decl1=get_decl_type(decl);
	free(decl1->subtrees[0]->value);
	decl1->subtrees[0]->value=new_name;

	add_decl(type,decl,0,0,0,1);

	c_write("add ",4);
	c_write(new_name,strlen(new_name));
	c_write(" ",1);
	new_name=get_decl_id(result.decl);
	c_write(new_name,strlen(new_name));
	c_write(" ",1);
	new_name=str_i_app(0,off);
	c_write(new_name,strlen(new_name));
	c_write("\n",1);
	free(new_name);

	ret->is_const=0;
	ret->is_lval=1;
	ret->needs_deref=1;
	ret->type=type;
	ret->decl=decl;
	expr_ret_release(&result);
}
