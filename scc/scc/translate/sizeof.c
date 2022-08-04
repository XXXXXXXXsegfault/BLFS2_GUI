void calculate_sizeof(struct syntax_tree *root,struct expr_ret *ret)
{
	int w;
	struct expr_ret result;
	struct syntax_tree *type,*decl;
	long int size;
	w=t_env.write;
	t_env.write=1;
	calculate_expr(root->subtrees[0],&result);
	if(result.needs_deref)
	{
		decl=decl_next(result.decl);
		syntax_tree_release(result.decl);
		result.decl=decl;
	}
	type=get_struct_member_list(result.type,0);
	if(type==0)
	{
		type=result.type;
	}
	size=type_size(type,result.decl);
	expr_ret_release(&result);
	t_env.write=w;
	ret->is_const=1;
	ret->value=size;
	ret->is_lval=0;
	ret->needs_deref=0;
	ret->decl=mkst("Identifier","<NULL>",root->line,root->col);
	ret->type=mkst("u64",0,root->line,root->col);
}
void calculate_sizeof_type(struct syntax_tree *root,struct expr_ret *ret)
{
	long int size;
	struct syntax_tree *type;
	type=get_struct_member_list(root->subtrees[0],0);
	if(type==0)
	{
		type=root->subtrees[0];
	}
	size=type_size(type,root->subtrees[1]);
	ret->is_const=1;
	ret->value=size;
	ret->is_lval=0;
	ret->needs_deref=0;
	ret->decl=mkst("Identifier","<NULL>",root->line,root->col);
	ret->type=mkst("u64",0,root->line,root->col);
}
