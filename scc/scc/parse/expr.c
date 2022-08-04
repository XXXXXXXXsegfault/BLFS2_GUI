struct syntax_tree *parse_id(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret;
	oldword=current;
	ret=0;
	if(current==0)
	{
		return 0;
	}
	if(cstr[0]>='A'&&cstr[0]<='Z'||cstr[0]>='a'&&cstr[0]<='z'||cstr[0]=='_')
	{
		if(!iskeyw(cstr))
		{
			ret=mkst("Identifier",cstr,line,col);
			next();
		}
	}
	if(ret==0)
	{
		resume();
		return 0;
	}
	return ret;
}
struct syntax_tree *parse_num_id(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret;
	oldword=current;
	ret=0;
	if(current==0)
	{
		return 0;
	}
	if(cstr[0]>='0'&&cstr[0]<='9'||cstr[0]=='\''||cstr[0]=='\"')
	{
		ret=mkst("Constant",cstr,line,col);
		next();
	}
	else if(cstr[0]>='A'&&cstr[0]<='Z'||cstr[0]>='a'&&cstr[0]<='z'||cstr[0]=='_')
	{
		if(!iskeyw(cstr))
		{
			ret=mkst("Identifier",cstr,line,col);
			next();
		}
	}
	if(ret==0)
	{
		resume();
		return 0;
	}
	return ret;
}
struct syntax_tree *parse_type(void);
struct syntax_tree *parse_decl(void);
struct syntax_tree *parse_expr_14(void);
struct syntax_tree *parse_expr_13(void);
struct syntax_tree *parse_expr_12(void);
struct syntax_tree *parse_expr_11(void);
struct syntax_tree *parse_expr_10(void);
struct syntax_tree *parse_expr_9(void);
struct syntax_tree *parse_expr_8(void);
struct syntax_tree *parse_expr_7(void);
struct syntax_tree *parse_expr_6(void);
struct syntax_tree *parse_expr_5(void);
struct syntax_tree *parse_expr_4(void);
struct syntax_tree *parse_expr_3(void);
struct syntax_tree *parse_expr_2(void);
struct syntax_tree *parse_expr_1(void);
struct syntax_tree *parse_expr_15(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node,*node2;
	oldword=current;
	ret=parse_expr_14();

	if(!ret)
	{
		resume();
		return 0;
	}
	while(!strcmp(cstr,","))
	{
		node2=mkst(",",0,line,col);
		next();
		node=parse_expr_14();
		if(!node)
		{
			error(line,col,"expected expression after \',\'.");
		}
		st_add_subtree(node2,ret);
		st_add_subtree(node2,node);
		ret=node2;
	}
	return ret;
}
char *expr_14_ops[12];
struct syntax_tree *parse_expr_14(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,**end,*node,*node2;
	char *op,*msg;
	oldword=current;
	ret=parse_expr_13();
	if(!ret)
	{
		resume();
		return 0;
	}
	end=&ret;
	while(op=str_list_match(expr_14_ops,cstr))
	{
		node2=mkst(op,0,line,col);
		next();
		st_add_subtree(node2,*end);
		node=parse_expr_13();
		if(node==0)
		{
			msg=xstrdup("expected expression after \'");
			msg=str_s_app(msg,op);
			msg=str_s_app(msg,"\'.");
			error(line,col,msg);
		}
		st_add_subtree(node2,node);
		*end=node2;
		end=node2->subtrees+1;
	}
	return ret;
}
struct syntax_tree *parse_expr_13(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,**end,*node,*node2;
	oldword=current;
	ret=parse_expr_12();
	if(!ret)
	{
		resume();
		return 0;
	}
	end=&ret;
	while(!strcmp(cstr,"\?"))
	{
		node2=mkst("?:",0,line,col);
		next();
		st_add_subtree(node2,*end);
		node=parse_expr_15();
		if(node==0)
		{
			error(line,col,"expected expression after \'\?\'.");
		}
		st_add_subtree(node2,node);
		if(strcmp(cstr,":"))
		{
			error(line,col,"expected \':\' after \'\?\'.");
		}
		next();
		node=parse_expr_12();
		if(node==0)
		{
			error(line,col,"expected expression after \':\'.");
		}
		st_add_subtree(node2,node);
		*end=node2;
		end=node2->subtrees+2;
	}
	return ret;
}
struct syntax_tree *parse_expr_12(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node,*node2;
	oldword=current;
	ret=parse_expr_11();
	if(!ret)
	{
		resume();
		return 0;
	}
	while(!strcmp(cstr,"||"))
	{
		node2=mkst("||",0,line,col);
		next();
		node=parse_expr_11();
		if(!node)
		{
			error(line,col,"expected expression after \'||\'.");
		}
		st_add_subtree(node2,ret);
		st_add_subtree(node2,node);
		ret=node2;
	}
	return ret;
}
struct syntax_tree *parse_expr_11(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node,*node2;
	oldword=current;
	ret=parse_expr_10();
	if(!ret)
	{
		resume();
		return 0;
	}
	while(!strcmp(cstr,"&&"))
	{
		node2=mkst("&&",0,line,col);
		next();
		node=parse_expr_10();
		if(!node)
		{
			error(line,col,"expected expression after \'&&\'.");
		}
		st_add_subtree(node2,ret);
		st_add_subtree(node2,node);
		ret=node2;
	}
	return ret;
}
struct syntax_tree *parse_expr_10(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node,*node2;
	oldword=current;
	ret=parse_expr_9();
	if(!ret)
	{
		resume();
		return 0;
	}
	while(!strcmp(cstr,"|"))
	{
		node2=mkst("|",0,line,col);
		next();
		node=parse_expr_9();
		if(!node)
		{
			error(line,col,"expected expression after \'|\'.");
		}
		st_add_subtree(node2,ret);
		st_add_subtree(node2,node);
		ret=node2;
	}
	return ret;
}
struct syntax_tree *parse_expr_9(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node,*node2;
	oldword=current;
	ret=parse_expr_8();
	if(!ret)
	{
		resume();
		return 0;
	}
	while(!strcmp(cstr,"^"))
	{
		node2=mkst("^",0,line,col);
		next();
		node=parse_expr_8();
		if(!node)
		{
			error(line,col,"expected expression after \'^\'.");
		}
		st_add_subtree(node2,ret);
		st_add_subtree(node2,node);
		ret=node2;
	}
	return ret;
}
struct syntax_tree *parse_expr_8(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node,*node2;
	oldword=current;
	ret=parse_expr_7();
	if(!ret)
	{
		resume();
		return 0;
	}
	while(!strcmp(cstr,"&"))
	{
		node2=mkst("&",0,line,col);
		next();
		node=parse_expr_7();
		if(!node)
		{
			error(line,col,"expected expression after \'&\'.");
		}
		st_add_subtree(node2,ret);
		st_add_subtree(node2,node);
		ret=node2;
	}
	return ret;
}
char *expr_7_ops[3];
struct syntax_tree *parse_expr_7(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node,*node2;
	char *op,*msg;
	oldword=current;
	ret=parse_expr_6();
	if(!ret)
	{
		resume();
		return 0;
	}
	while(op=str_list_match(expr_7_ops,cstr))
	{
		node2=mkst(op,0,line,col);
		next();
		node=parse_expr_6();
		if(!node)
		{
			msg=xstrdup("expected expression after \'");
			msg=str_s_app(msg,op);
			msg=str_s_app(msg,"\'.");
			error(line,col,msg);
		}
		st_add_subtree(node2,ret);
		st_add_subtree(node2,node);
		ret=node2;
	}
	return ret;
}
char *expr_6_ops[5];
struct syntax_tree *parse_expr_6(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node,*node2;
	char *op,*msg;
	oldword=current;
	ret=parse_expr_5();
	if(!ret)
	{
		resume();
		return 0;
	}
	while(op=str_list_match(expr_6_ops,cstr))
	{
		node2=mkst(op,0,line,col);
		next();
		node=parse_expr_5();
		if(!node)
		{
			msg=xstrdup("expected expression after \'");
			msg=str_s_app(msg,op);
			msg=str_s_app(msg,"\'.");
			error(line,col,msg);
		}
		st_add_subtree(node2,ret);
		st_add_subtree(node2,node);
		ret=node2;
	}
	return ret;
}
char *expr_5_ops[3];
struct syntax_tree *parse_expr_5(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node,*node2;
	char *op,*msg;
	oldword=current;
	ret=parse_expr_4();
	if(!ret)
	{
		resume();
		return 0;
	}
	while(op=str_list_match(expr_5_ops,cstr))
	{
		node2=mkst(op,0,line,col);
		next();
		node=parse_expr_4();
		if(!node)
		{
			msg=xstrdup("expected expression after \'");
			msg=str_s_app(msg,op);
			msg=str_s_app(msg,"\'.");
			error(line,col,msg);
		}
		st_add_subtree(node2,ret);
		st_add_subtree(node2,node);
		ret=node2;
	}
	return ret;
}
char *expr_4_ops[3];
struct syntax_tree *parse_expr_4(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node,*node2;
	char *op,*msg;
	oldword=current;
	ret=parse_expr_3();
	if(!ret)
	{
		resume();
		return 0;
	}
	while(op=str_list_match(expr_4_ops,cstr))
	{
		node2=mkst(op,0,line,col);
		next();
		node=parse_expr_3();
		if(!node)
		{
			msg=xstrdup("expected expression after \'");
			msg=str_s_app(msg,op);
			msg=str_s_app(msg,"\'.");
			error(line,col,msg);
		}
		st_add_subtree(node2,ret);
		st_add_subtree(node2,node);
		ret=node2;
	}
	return ret;
}
char *expr_3_ops[4];
struct syntax_tree *parse_expr_3(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node,*node2;
	char *op,*msg;
	oldword=current;
	ret=parse_expr_2();
	if(!ret)
	{
		resume();
		return 0;
	}
	while(op=str_list_match(expr_3_ops,cstr))
	{
		node2=mkst(op,0,line,col);
		next();
		node=parse_expr_2();
		if(!node)
		{
			msg=xstrdup("expected expression after \'");
			msg=str_s_app(msg,op);
			msg=str_s_app(msg,"\'.");
			error(line,col,msg);
		}
		st_add_subtree(node2,ret);
		st_add_subtree(node2,node);
		ret=node2;
	}
	return ret;
}
struct syntax_tree *parse_sizeof_type(void)
{
	struct l_word_list *oldword;
	long int l,c;
	struct syntax_tree *node,*ret;
	oldword=current;
	l=line;
	c=col;
	if(strcmp(cstr,"sizeof"))
	{
		return 0;
	}
	next();
	if(strcmp(cstr,"("))
	{
		resume();
		return 0;
	}
	next();
	node=parse_type();
	if(node==0)
	{
		resume();
		return 0;
	}
	ret=mkst("sizeof_type",0,l,c);
	st_add_subtree(ret,node);
	node=parse_decl();
	if(node==0)
	{
		error(line,col,"invalid declaration.");
	}
	if(strcmp(cstr,")"))
	{
		error(line,col,"expected \')\' after declaration.");
	}
	next();
	st_add_subtree(ret,node);
	return ret;
}
struct syntax_tree *parse_cast(void)
{
	struct l_word_list *oldword;
	long int l,c;
	struct syntax_tree *node,*ret;
	oldword=current;
	l=line;
	c=col;
	if(strcmp(cstr,"("))
	{
		resume();
		return 0;
	}
	next();
	node=parse_type();
	if(node==0)
	{
		resume();
		return 0;
	}
	ret=mkst("cast",0,l,c);
	st_add_subtree(ret,node);
	node=parse_decl();
	if(node==0)
	{
		error(line,col,"invalid declaration.");
	}
	if(strcmp(cstr,")"))
	{
		error(line,col,"expected \')\' after declaration.");
	}
	next();
	st_add_subtree(ret,node);
	return ret;
}
char *expr_2_ops[9];
struct syntax_tree *parse_expr_2(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node;
	char *op,*msg;
	oldword=current;
	if(ret=parse_sizeof_type())
	{
		return ret;
	}
	if(op=str_list_match(expr_2_ops,cstr))
	{
		if(!strcmp(op,"-"))
		{
			op="neg";
		}
		else if(!strcmp(op,"*"))
		{
			op="deref";
		}
		else if(!strcmp(op,"&"))
		{
			op="addr";
		}
		ret=mkst(op,0,line,col);
		next();
		node=parse_expr_2();
		if(!node)
		{
			msg=xstrdup("expected expression after \'");
			msg=str_s_app(msg,op);
			msg=str_s_app(msg,"\'.");
			error(line,col,msg);
		}
		st_add_subtree(ret,node);
	}
	else if(ret=parse_cast())
	{
		node=parse_expr_2();
		if(!node)
		{
			error(line,col,"expected expression after \')\'.");
		}
		st_add_subtree(ret,node);
	}
	else
	{
		ret=parse_expr_1();
		if(ret==0)
		{
			resume();
			return 0;
		}
	}
	return ret;
}
struct syntax_tree *parse_call(void)
{
	struct syntax_tree *ret,*node;
	ret=0;
	if(!strcmp(cstr,"("))
	{
		next();
		node=parse_expr_14();
		if(node==0)
		{
			ret=mkst("call_noarg",0,line,col);
			if(strcmp(cstr,")"))
			{
				error(line,col,"expected \')\' after \'(\'.");
			}
			next();
			st_add_subtree(ret,0);
		}
		else
		{
			ret=mkst("call",0,line,col);
			st_add_subtree(ret,0);
			st_add_subtree(ret,node);
			while(!strcmp(cstr,","))
			{
				next();
				node=parse_expr_14();
				if(node==0)
				{
					error(line,col,"expected expression after \',\'.");
				}
				st_add_subtree(ret,node);
			}
			if(strcmp(cstr,")"))
			{
				error(line,col,"expected \')\' after \'(\'.");
			}
			next();
		}
	}
	return ret;
}
struct syntax_tree *parse_expr_1_suffix(void)
{
	struct syntax_tree *ret,*node;
	ret=0;
	if(!strcmp(cstr,"["))
	{
		ret=mkst("[]",0,line,col);
		next();
		node=parse_expr_15();
		if(node==0)
		{
			error(line,col,"expected expression after \'(\'.");
		}
		if(strcmp(cstr,"]"))
		{
			error(line,col,"expected \']\' after \'[\'.");
		}
		next();
		st_add_subtree(ret,0);
		st_add_subtree(ret,node);
	}
	else if(ret=parse_call())
	{
		return ret;
	}
	else if(!strcmp(cstr,"."))
	{
		ret=mkst(".",0,line,col);
		next();
		node=parse_id();
		if(!node)
		{
			error(line,col,"expected member name after \'.\'.");
		}
		st_add_subtree(ret,0);
		st_add_subtree(ret,node);
	}
	else if(!strcmp(cstr,"->"))
	{
		ret=mkst("->",0,line,col);
		next();
		node=parse_id();
		if(!node)
		{
			error(line,col,"expected member name after \'->\'.");
		}
		st_add_subtree(ret,0);
		st_add_subtree(ret,node);
	}
	return ret;
}
struct syntax_tree *parse_expr_1(void)
{
	struct syntax_tree *ret,*node;
	if(!strcmp(cstr,"("))
	{
		next();
		ret=parse_expr_15();
		if(ret==0)
		{
			error(line,col,"expected expression after \'(\'.");
		}
		if(strcmp(cstr,")"))
		{
			error(line,col,"expected \')\' after \'(\'.");
		}
		next();
	}
	else
	{
		ret=parse_num_id();
	}
	while(node=parse_expr_1_suffix())
	{
		node->subtrees[0]=ret;
		ret=node;
	}
	return ret;
}
struct syntax_tree *parse_expr(void)
{
	struct syntax_tree *ret,*node;
	if(node=parse_expr_15())
	{
		if(strcmp(cstr,";"))
		{
			error(line,col,"expected \';\' after expression.");
		}
		ret=mkst("expr",0,line,col);
		next();
		st_add_subtree(ret,node);
		return ret;
	}
	return 0;
}
void expr_global_init(void)
{
	expr_14_ops[0]="=";
	expr_14_ops[1]="+=";
	expr_14_ops[2]="-=";
	expr_14_ops[3]="*=";
	expr_14_ops[4]="/=";
	expr_14_ops[5]="%=";
	expr_14_ops[6]="&=";
	expr_14_ops[7]="|=";
	expr_14_ops[8]="^=";
	expr_14_ops[9]="<<=";
	expr_14_ops[10]=">>=";
	expr_7_ops[0]="==";
	expr_7_ops[1]="!=";
	expr_6_ops[0]=">";
	expr_6_ops[1]=">=";
	expr_6_ops[2]="<";
	expr_6_ops[3]="<=";
	expr_5_ops[0]="<<";
	expr_5_ops[1]=">>";
	expr_4_ops[0]="+";
	expr_4_ops[1]="-";
	expr_3_ops[0]="/";
	expr_3_ops[1]="*";
	expr_3_ops[2]="%";
	expr_2_ops[0]="-";
	expr_2_ops[1]="++";
	expr_2_ops[2]="--";
	expr_2_ops[3]="*";
	expr_2_ops[4]="&";
	expr_2_ops[5]="!";
	expr_2_ops[6]="~";
	expr_2_ops[7]="sizeof";
}
