struct branch_args
{
	long int ltrue;
	long int lfalse;
};
void write_label_name(long int num)
{
	char *str;
	str=str_i_app(0,num);
	c_write(str,strlen(str));
	free(str);
}
void write_label(long int num)
{
	c_write("label ",6);
	write_label_name(num);
	c_write("\n",1);
}
void translate_branch(struct syntax_tree *root,struct branch_args *args);
void translate_branch_relop(struct syntax_tree *root,struct branch_args *args,char *ins1,char *ins2)
{
	struct expr_ret left,right;
	char *left_name,*right_name;
	int val;
	calculate_expr(root->subtrees[0],&left);
	calculate_expr(root->subtrees[1],&right);
	deref_ptr(&left,root->line,root->col);
	deref_ptr(&right,root->line,root->col);
	if(!left.is_const&&!right.is_const)
	{
		if(if_type_compat(left.type,left.decl,right.type,right.decl,0))
		{
			error(root->line,root->col,"incompatible type.");
		}
	}
	if(left.is_const&&right.is_const)
	{
		val=0;
		if(!strcmp(ins1,"bgt "))
		{
			if(left.value>right.value)
			{
				val=1;
			}
		}
		else if(!strcmp(ins1,"blt "))
		{
			if(left.value<right.value)
			{
				val=1;
			}
		}
		else if(!strcmp(ins1,"bge "))
		{
			if(left.value>=right.value)
			{
				val=1;
			}
		}
		else if(!strcmp(ins1,"ble "))
		{
			if(left.value<=right.value)
			{
				val=1;
			}
		}
		else if(!strcmp(ins1,"beq "))
		{
			if(left.value==right.value)
			{
				val=1;
			}
		}
		else if(!strcmp(ins1,"bne "))
		{
			if(left.value!=right.value)
			{
				val=1;
			}
		}
		if(val&&args->ltrue!=-1)
		{
			c_write("bal ",4);
			write_label_name(args->ltrue);
			c_write("\n",1);
		}
		else if(!val&&args->lfalse!=-1)
		{
			c_write("bal ",4);
			write_label_name(args->lfalse);
			c_write("\n",1);
		}
		expr_ret_release(&left);
		expr_ret_release(&right);
		return;
	}
	if(left.is_const)
	{
		left_name=str_i_app(0,left.value);
	}
	else
	{
		left_name=get_decl_id(left.decl);
	}
	if(right.is_const)
	{
		right_name=str_i_app(0,right.value);
	}
	else
	{
		right_name=get_decl_id(right.decl);
	}
	if(args->ltrue!=-1)
	{
		c_write(ins1,4);
		c_write(left_name,strlen(left_name));
		c_write(" ",1);
		c_write(right_name,strlen(right_name));
		c_write(" ",1);
		write_label_name(args->ltrue);
		c_write("\n",1);
		if(args->lfalse!=-1)
		{
			c_write("bal ",4);
			write_label_name(args->lfalse);
			c_write("\n",1);
		}
	}
	else if(args->lfalse!=-1)
	{
		c_write(ins2,4);
		c_write(left_name,strlen(left_name));
		c_write(" ",1);
		c_write(right_name,strlen(right_name));
		c_write(" ",1);
		write_label_name(args->lfalse);
		c_write("\n",1);
	}
	expr_ret_release(&left);
	expr_ret_release(&right);
}
void translate_branch_and(struct syntax_tree *root,struct branch_args *args)
{
	struct branch_args left,right;
	control_label_push();
	right.ltrue=args->ltrue;
	right.lfalse=args->lfalse;
	if(args->lfalse==-1)
	{
		args->lfalse=t_env.label->l3;
	}
	left.ltrue=-1;
	left.lfalse=args->lfalse;
	translate_branch(root->subtrees[0],&left);
	translate_branch(root->subtrees[1],&right);
	write_label(t_env.label->l3);
	control_label_pop();
}
void translate_branch_or(struct syntax_tree *root,struct branch_args *args)
{
	struct branch_args left,right;
	int s;
	control_label_push();
	right.ltrue=args->ltrue;
	right.lfalse=args->lfalse;
	if(args->ltrue==-1)
	{
		args->ltrue=t_env.label->l3;
	}
	left.ltrue=args->ltrue;
	left.lfalse=-1;
	translate_branch(root->subtrees[0],&left);
	translate_branch(root->subtrees[1],&right);
	write_label(t_env.label->l3);
	control_label_pop();
}
void translate_branch_not(struct syntax_tree *root,struct branch_args *args)
{
	struct branch_args arg;
	arg.ltrue=args->lfalse;
	arg.lfalse=args->ltrue;
	translate_branch(root->subtrees[0],&arg);
}
void translate_branch(struct syntax_tree *root,struct branch_args *args)
{
	struct expr_ret result;
	char *name;
	if(!strcmp(root->name,"<="))
	{
		translate_branch_relop(root,args,"ble ","bgt ");
	}
	else if(!strcmp(root->name,">="))
	{
		translate_branch_relop(root,args,"bge ","blt ");
	}
	else if(!strcmp(root->name,"<"))
	{
		translate_branch_relop(root,args,"blt ","bge ");
	}
	else if(!strcmp(root->name,">"))
	{
		translate_branch_relop(root,args,"bgt ","ble ");
	}
	else if(!strcmp(root->name,"=="))
	{
		translate_branch_relop(root,args,"beq ","bne ");
	}
	else if(!strcmp(root->name,"!="))
	{
		translate_branch_relop(root,args,"bne ","beq ");
	}
	else if(!strcmp(root->name,"!"))
	{
		translate_branch_not(root,args);
	}
	else if(!strcmp(root->name,"&&"))
	{
		translate_branch_and(root,args);
	}
	else if(!strcmp(root->name,"||"))
	{
		translate_branch_or(root,args);
	}
	else
	{
		calculate_expr(root,&result);
		deref_ptr(&result,root->line,root->col);
		if(result.is_const)
		{
			if(result.value&&args->ltrue!=-1)
			{
				c_write("bal ",4);
				write_label_name(args->ltrue);
				c_write("\n",1);
			}
			else if(!result.value&&args->lfalse!=-1)
			{
				c_write("bal ",4);
				write_label_name(args->lfalse);
				c_write("\n",1);
			}
			expr_ret_release(&result);
			return;
		}
		name=get_decl_id(result.decl);
		if(args->ltrue!=-1)
		{
			c_write("bne ",4);
			c_write(name,strlen(name));
			c_write(" 0 ",3);
			write_label_name(args->ltrue);
			c_write("\n",1);
			if(args->lfalse!=-1)
			{
				c_write("bal ",4);
				write_label_name(args->lfalse);
				c_write("\n",1);
			}
		}
		else if(args->lfalse!=-1)
		{
			c_write("beq ",4);
			c_write(name,strlen(name));
			c_write(" 0 ",3);
			write_label_name(args->lfalse);
			c_write("\n",1);
		}
		expr_ret_release(&result);
	}
}
