void calculate_assign_op(struct syntax_tree *root,struct expr_ret *ret,char *op,char *ins)
{
	struct expr_ret left,right;
	struct syntax_tree *decl1,*decl2;
	long int size;
	struct syntax_tree *new_type,*new_decl;
	char *new_name,*old_name,*str;
	char *msg;
	calculate_expr(root->subtrees[0],&left);
	calculate_expr(root->subtrees[1],&right);
	if(left.is_lval==0)
	{
		error(root->line,root->col,"lvalue required here.");
	}
	deref_ptr(&right,root->line,root->col);
	if(!is_basic_decl(right.decl))
	{
		msg=xstrdup("invalid operand for \'");
		msg=str_s_app(msg,op);
		msg=str_s_app(msg,"\'.");
		error(root->line,root->col,msg);
	}
	if(left.needs_deref)
	{
		new_name=mktmpname();
		decl1=decl_next(left.decl);
		if(is_basic_type(left.type)&&is_basic_decl(decl1))
		{
			size=type_size(left.type,decl1);
		}
		else
		{
			msg=xstrdup("invalid operand for \'");
			msg=str_s_app(msg,op);
			msg=str_s_app(msg,"\'.");
			error(root->line,root->col,msg);
		}
		
		new_type=syntax_tree_dup(left.type);
		new_decl=syntax_tree_dup(left.decl);
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

		if(right.is_const)
		{
			str=str_i_app(0,right.value);
		}
		else
		{
			str=xstrdup(get_decl_id(right.decl));
		}
		c_write(ins,strlen(ins));
		c_write(" ",1);
		c_write(new_name,strlen(new_name));
		c_write(" ",1);
		c_write(new_name,strlen(new_name));
		c_write(" ",1);


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
		syntax_tree_release(decl1);

		ret->type=new_type;
		ret->decl=new_decl;
		ret->is_const=0;
		ret->is_lval=0;
		ret->needs_deref=0;
	}
	else
	{

		if(right.is_const)
		{
			str=str_i_app(0,right.value);
		}
		else
		{
			str=xstrdup(get_decl_id(right.decl));
		}

		old_name=get_decl_id(left.decl);
		c_write(ins,strlen(ins));
		c_write(" ",1);
		c_write(old_name,strlen(old_name));
		c_write(" ",1);
		c_write(old_name,strlen(old_name));
		c_write(" ",1);
		c_write(str,strlen(str));
		c_write("\n",1);
		free(str);
		ret->type=syntax_tree_dup(left.type);
		ret->decl=syntax_tree_dup(left.decl);
		ret->is_const=0;
		ret->is_lval=0;
		ret->needs_deref=0;
	}
	expr_ret_release(&left);
	expr_ret_release(&right);
}
