void calculate_assign(struct syntax_tree *root,struct expr_ret *ret,char *op1,char *op2)
{
	struct expr_ret left,right;
	char *str,*name,*tname;
	struct syntax_tree *decl1,*t;
	int size;
	calculate_expr(root->subtrees[0],&left);
	calculate_expr(root->subtrees[1],&right);
	if(left.is_lval==0||is_array_function(left.decl))
	{
		error(root->line,root->col,"lvalue required here.");
	}
	if(right.is_const)
	{
		str=str_i_app(0,right.value);
	}
	else
	{
		deref_ptr(&right,root->line,root->col);
		str=xstrdup(get_decl_id(right.decl));
	}
	name=get_decl_id(left.decl);
	if(left.needs_deref)
	{
		decl1=decl_next(left.decl);
		if(if_type_compat(left.type,decl1,right.type,right.decl,0))
		{
			error(root->line,root->col,"incompatible type.");
		}
		t=get_decl_type(decl1);
		c_write(op2,strlen(op2));
		if(!strcmp(t->name,"pointer"))
		{
			c_write("q ",2);
		}
		else if(!strcmp(t->name,"Identifier"))
		{
			if(!strcmp(left.type->name,"struct"))
			{
				error(root->line,root->col,"invalid use of structure.");
			}
			if(!strcmp(left.type->name,"union"))
			{
				error(root->line,root->col,"invalid use of union.");
			}
			size=type_size(left.type,decl1);
			if(size==1)
			{
				c_write("b ",2);
			}
			else if(size==2)
			{
				c_write("w ",2);
			}
			else if(size==4)
			{
				c_write("l ",2);
			}
			else if(size==8)
			{
				c_write("q ",2);
			}
			else
			{
				error(root->line,root->col,"invalid assignment.");
			}
		}
		else
		{
			error(root->line,root->col,"invalid assignment.");
		}
		syntax_tree_release(decl1);
	}
	else
	{
		if(if_type_compat(left.type,left.decl,right.type,right.decl,0))
		{
			error(root->line,root->col,"incompatible type.");
		}
		c_write(op1,strlen(op1));
		c_write(" ",1);
		if(strcmp(op1,"mov"))
		{
			c_write(name,strlen(name));
			c_write(" ",1);
		}
	}
	c_write(name,strlen(name));
	c_write(" ",1);
	c_write(str,strlen(str));
	c_write("\n",1);
	free(str);
	ret->is_lval=0;
	ret->is_const=right.is_const;
	ret->decl=syntax_tree_dup(right.decl);
	ret->type=syntax_tree_dup(right.type);
	ret->needs_deref=0;
	if(ret->is_const)
	{
		ret->value=right.value;
	}
	expr_ret_release(&left);
	expr_ret_release(&right);
}
