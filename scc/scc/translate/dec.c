void calculate_dec(struct syntax_tree *root,struct expr_ret *ret)
{
	struct expr_ret result;
	struct syntax_tree *decl1,*decl2;
	long int scale,size;
	struct syntax_tree *new_type,*new_decl;
	char *new_name,*old_name,*str;
	scale=1;
	calculate_expr(root->subtrees[0],&result);
	if(result.is_lval==0||is_array_function(result.decl))
	{
		error(root->line,root->col,"lvalue required here.");
	}
	if(result.needs_deref)
	{
		new_name=mktmpname();
		decl1=decl_next(result.decl);
		if(is_pointer_array(decl1))
		{
			decl2=decl_next(decl1);
			scale=type_size(result.type,decl2);
			if(scale==0)
			{
				scale=1;
			}
			size=8;
			syntax_tree_release(decl2);
		}
		else if(is_basic_type(result.type)&&is_basic_decl(decl1))
		{
			size=type_size(result.type,decl1);
		}
		else
		{
			error(root->line,root->col,"invalid operand for \'--\'.");
		}
		new_type=syntax_tree_dup(result.type);
		new_decl=syntax_tree_dup(result.decl);
		decl2=get_decl_type(new_decl);
		if(!strcmp(decl2->name,"Identifier"))
		{
			old_name=decl2->value;
			decl2->value=new_name;
		}
		else
		{
			old_name=decl2->subtrees[0]->value;
			decl2->subtrees[0]->value=new_name;
		}
		add_decl(new_type,new_decl,0,0,0,1);
		if(size==1)
		{
			c_write("ldb ",4);
		}
		else if(size==2)
		{
			c_write("ldw ",4);
		}
		else if(size==4)
		{
			c_write("ldl ",4);
		}
		else if(size==8)
		{
			c_write("ldq ",4);
		}
		c_write(new_name,strlen(new_name));
		c_write(" ",1);
		c_write(old_name,strlen(old_name));
		c_write("\n",1);

		c_write("sub ",4);
		c_write(new_name,strlen(new_name));
		c_write(" ",1);
		c_write(new_name,strlen(new_name));
		c_write(" ",1);

		str=str_i_app(0,scale);
		c_write(str,strlen(str));
		c_write("\n",1);
		free(str);
		if(size==1)
		{
			c_write("stb ",4);
		}
		else if(size==2)
		{
			c_write("stw ",4);
		}
		else if(size==4)
		{
			c_write("stl ",4);
		}
		else if(size==8)
		{
			c_write("stq ",4);
		}
		c_write(old_name,strlen(old_name));
		c_write(" ",1);
		c_write(new_name,strlen(new_name));
		c_write("\n",1);

		free(old_name);
		syntax_tree_release(decl1);

		ret->type=new_type;
		ret->decl=new_decl;
		ret->is_const=0;
		ret->is_lval=0;
		ret->needs_deref=0;
	}
	else
	{
		if(is_pointer_array(result.decl))
		{
			decl2=decl_next(result.decl);
			scale=type_size(result.type,decl2);
			if(scale==0)
			{
				scale=1;
			}
			syntax_tree_release(decl2);
		}
		old_name=get_decl_id(result.decl);
		c_write("sub ",4);
		c_write(old_name,strlen(old_name));
		c_write(" ",1);
		c_write(old_name,strlen(old_name));
		c_write(" ",1);
		str=str_i_app(0,scale);
		c_write(str,strlen(str));
		c_write("\n",1);
		free(str);
		ret->type=syntax_tree_dup(result.type);
		ret->decl=syntax_tree_dup(result.decl);
		ret->is_const=0;
		ret->is_lval=0;
		ret->needs_deref=0;
	}
	expr_ret_release(&result);
}
