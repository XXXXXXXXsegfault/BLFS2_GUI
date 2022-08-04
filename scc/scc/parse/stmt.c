struct syntax_tree *parse_stmt(void);
struct syntax_tree *parse_asm(void)
{
	struct syntax_tree *ret,*node;
	if(strcmp(cstr,"asm"))
	{
		return 0;
	}
	ret=mkst("asm",0,line,col);
	next();
	if(cstr[0]!='\"')
	{
		error(line,col,"expected string after \'asm\'.");
	}
	node=mkst("asm_str",cstr,line,col);
	next();
	st_add_subtree(ret,node);
	return ret;
}
struct syntax_tree *parse_init(void)
{
	struct syntax_tree *ret,*node;
	if(!strcmp(cstr,"{"))
	{
		ret=mkst("init",0,line,col);
		next();
		while(1)
		{
			node=parse_init();
			if(node==0)
			{
				error(line,col,"invalid expression in initializer.");
			}
			st_add_subtree(ret,node);
			if(strcmp(cstr,","))
			{
				break;
			}
			next();
		}
		if(strcmp(cstr,"}"))
		{
			error(line,col,"expected \'}\' after expression.");
		}
		next();
		return ret;
	}
	return parse_expr_14();
}
struct syntax_tree *parse_decl_stmt(void)
{
	struct syntax_tree *ret,*node,*t;
	if(node=parse_type())
	{
		ret=mkst("decl",0,line,col);
		st_add_subtree(ret,node);
		while(1)
		{
			node=parse_decl();
			if(!node)
			{
				error(line,col,"invalid declaration.");
			}
			st_add_subtree(ret,node);
			if(!strcmp(cstr,"="))
			{
				next();
				node=parse_init();
				if(!node)
				{
					error(line,col,"invalid initializer.");
				}
				t=mkst("Init",0,line,col);
				st_add_subtree(t,node);
				st_add_subtree(ret,t);
			}
			if(strcmp(cstr,","))
			{
				break;
			}
			next();
		}
		if(strcmp(cstr,";"))
		{
			error(line,col,"expected \';\' after declarations.");
		}
		next();
		return ret;
	}
	return 0;
}
struct syntax_tree *parse_extern_decl_stmt(void)
{
	struct syntax_tree *ret,*node;
	if(strcmp(cstr,"extern"))
	{
		return 0;
	}
	next();
	if(node=parse_type())
	{
		ret=mkst("extern_decl",0,line,col);
		st_add_subtree(ret,node);
		while(1)
		{
			node=parse_decl();
			if(!node)
			{
				error(line,col,"invalid declaration.");
			}
			st_add_subtree(ret,node);
			if(!strcmp(cstr,"="))
			{
				next();
				node=parse_init();
				if(!node)
				{
					error(line,col,"invalid initializer.");
				}
				st_add_subtree(ret,node);
			}
			if(strcmp(cstr,","))
			{
				break;
			}
			next();
		}
		if(strcmp(cstr,";"))
		{
			error(line,col,"expected \';\' after declarations.");
		}
		next();
		return ret;
	}
	return 0;
}
struct syntax_tree *parse_static_decl_stmt(void)
{
	struct syntax_tree *ret,*node;
	if(strcmp(cstr,"static"))
	{
		return 0;
	}
	next();
	if(node=parse_type())
	{
		ret=mkst("static_decl",0,line,col);
		st_add_subtree(ret,node);
		while(1)
		{
			node=parse_decl();
			if(!node)
			{
				error(line,col,"invalid declaration.");
			}
			st_add_subtree(ret,node);
			if(!strcmp(cstr,"="))
			{
				next();
				node=parse_init();
				if(!node)
				{
					error(line,col,"invalid initializer.");
				}
				st_add_subtree(ret,node);
			}
			if(strcmp(cstr,","))
			{
				break;
			}
			next();
		}
		if(strcmp(cstr,";"))
		{
			error(line,col,"expected \';\' after declarations.");
		}
		next();
		return ret;
	}
	return 0;
}
struct syntax_tree *parse_stmt_block(void)
{
	struct syntax_tree *ret,*node;
	if(strcmp(cstr,"{"))
	{
		return 0;
	}
	ret=mkst("block",0,line,col);
	next();
	while(node=parse_stmt())
	{
		st_add_subtree(ret,node);
	}
	if(strcmp(cstr,"}"))
	{
		error(line,col,"expected \'}\' after statement.");
	}
	next();
	return ret;
}
struct syntax_tree *parse_if_stmt(void)
{
	struct syntax_tree *ret,*node;
	if(strcmp(cstr,"if"))
	{
		return 0;
	}
	ret=mkst("if",0,line,col);
	next();
	if(strcmp(cstr,"("))
	{
		error(line,col,"expected \'(\' after \'if\'.");
	}
	next();
	node=parse_expr_15();
	if(!node)
	{
		error(line,col,"expected expression after \'(\'.");
	}
	st_add_subtree(ret,node);
	if(strcmp(cstr,")"))
	{
		error(line,col,"expected \')\' after expression.");
	}
	next();
	node=parse_stmt();
	if(!node)
	{
		error(line,col,"invalid statement.");
	}
	st_add_subtree(ret,node);
	if(!strcmp(cstr,"else"))
	{
		next();
		node=parse_stmt();
		if(!node)
		{
			error(line,col,"invalid statement.");
		}
		st_add_subtree(ret,node);
		ret->name="ifelse";
	}
	return ret;
}
struct syntax_tree *parse_while_stmt(void)
{
	struct syntax_tree *ret,*node;
	if(strcmp(cstr,"while"))
	{
		return 0;
	}
	ret=mkst("while",0,line,col);
	next();
	if(strcmp(cstr,"("))
	{
		error(line,col,"expected \'(\' after \'while\'.");
	}
	next();
	node=parse_expr_15();
	if(!node)
	{
		error(line,col,"expected expression after \'(\'.");
	}
	st_add_subtree(ret,node);
	if(strcmp(cstr,")"))
	{
		error(line,col,"expected \')\' after expression.");
	}
	next();
	node=parse_stmt();
	if(!node)
	{
		error(line,col,"invalid statement.");
	}
	st_add_subtree(ret,node);
	return ret;
}
struct syntax_tree *parse_dowhile_stmt(void)
{
	struct syntax_tree *ret,*node;
	if(strcmp(cstr,"do"))
	{
		return 0;
	}
	ret=mkst("dowhile",0,line,col);
	next();
	node=parse_stmt();
	if(!node)
	{
		error(line,col,"invalid statement.");
	}
	st_add_subtree(ret,node);
	if(strcmp(cstr,"while"))
	{
		error(line,col,"expected \'while\' after \'do\'.");
	}
	next();
	if(strcmp(cstr,"("))
	{
		error(line,col,"expected \'(\' after \'while\'.");
	}
	next();
	node=parse_expr_15();
	if(!node)
	{
		error(line,col,"expected expression after \'(\'.");
	}
	st_add_subtree(ret,node);
	if(strcmp(cstr,")"))
	{
		error(line,col,"expected \')\' after expression.");
	}
	next();
	if(strcmp(cstr,";"))
	{
		error(line,col,"expected \';\' after \')\'.");
	}
	next();
	return ret;
}
struct syntax_tree *parse_return_stmt(void)
{
	struct syntax_tree *ret,*node;
	if(strcmp(cstr,"return"))
	{
		return 0;
	}
	ret=mkst("return",0,line,col);
	next();
	if(node=parse_expr_15())
	{
		st_add_subtree(ret,node);
	}
	if(strcmp(cstr,";"))
	{
		error(line,col,"expected \';\' after expression.");
	}
	next();
	return ret;
}
struct syntax_tree *parse_break_stmt(void)
{
	struct syntax_tree *ret,*node;
	if(strcmp(cstr,"break"))
	{
		return 0;
	}
	ret=mkst("break",0,line,col);
	next();
	if(strcmp(cstr,";"))
	{
		error(line,col,"expected \';\' after expression.");
	}
	next();
	return ret;
}
struct syntax_tree *parse_goto_stmt(void)
{
	struct syntax_tree *ret,*node;
	if(strcmp(cstr,"goto"))
	{
		return 0;
	}
	ret=mkst("goto",0,line,col);
	next();
	node=parse_id();
	if(!node)
	{
		error(line,col,"expected label name after \'goto\'.");
	}
	st_add_subtree(ret,node);
	if(strcmp(cstr,";"))
	{
		error(line,col,"expected \';\' after expression.");
	}
	next();
	return ret;
}
struct syntax_tree *parse_stmt(void)
{
	struct syntax_tree *ret;
	if(ret=parse_stmt_block())
	{
		return ret;
	}
	if(ret=parse_static_decl_stmt())
	{
		return ret;
	}
	if(ret=parse_extern_decl_stmt())
	{
		return ret;
	}
	if(ret=parse_decl_stmt())
	{
		return ret;
	}
	if(ret=parse_expr())
	{
		return ret;
	}
	if(ret=parse_asm())
	{
		return ret;
	}
	if(ret=parse_if_stmt())
	{
		return ret;
	}
	if(ret=parse_while_stmt())
	{
		return ret;
	}
	if(ret=parse_dowhile_stmt())
	{
		return ret;
	}
	if(ret=parse_return_stmt())
	{
		return ret;
	}
	if(ret=parse_break_stmt())
	{
		return ret;
	}
	if(ret=parse_goto_stmt())
	{
		return ret;
	}
	if(!strcmp(cstr,";"))
	{
		ret=mkst("null",0,line,col);
		next();
		return ret;
	}
	return 0;
}
struct syntax_tree *parse_fundef(void)
{
	int n;
	struct syntax_tree *ret,*node,*t;
	n=0;
	if(node=parse_type())
	{
		ret=mkst("decl",0,line,col);
		st_add_subtree(ret,node);
		while(1)
		{
			node=parse_decl();
			if(!node)
			{
				error(line,col,"invalid declaration.");
			}
			st_add_subtree(ret,node);
			if(!strcmp(cstr,"="))
			{
				next();
				node=parse_init();
				if(!node)
				{
					error(line,col,"invalid initializer.");
				}
				t=mkst("Init",0,line,col);
				st_add_subtree(t,node);
				st_add_subtree(ret,t);
			}
			++n;
			if(strcmp(cstr,","))
			{
				break;
			}
			next();
		}
		if(strcmp(cstr,";"))
		{
			if(n==1&&!strcmp(cstr,"{"))
			{
				node=parse_stmt_block();
				if(node==0)
				{
					error(line,col,"invalid function definition.");
				}
				st_add_subtree(ret,node);
				ret->name="fundef";
				return ret;
			}
			error(line,col,"expected \';\' after declarations.");
		}
		next();
		return ret;
	}
	return 0;
}
