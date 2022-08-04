char *basic_type_str[7];
struct syntax_tree *parse_decl(void);
struct syntax_tree *parse_type(void);
struct syntax_tree *parse_id_null(void)
{
	struct syntax_tree *ret;
	if(ret=parse_id())
	{
		return ret;
	}
	return mkst("Identifier","<NULL>",line,col);
}
struct syntax_tree *parse_basic_type(void)
{
	int t[6];
	int x;
	long int l,c;
	l=line;
	c=col;
	t[0]=0;
	t[1]=0;
	t[2]=0;
	t[3]=0;
	t[4]=0;
	t[5]=0;
	if(!strcmp(cstr,"void"))
	{
		next();
		return mkst("void",0,l,c);
	}
	while((x=str_list_match2(basic_type_str,cstr))!=-1)
	{
		++t[x];
		next();
	}
	if(t[0]>1||t[1]>1||t[2]>1||t[3]>2||t[4]>1||t[5]>1)
	{
		error(line,col,"invalid type.");
	}
	if(t[0]+t[1]>1||t[4]+t[5]>1||t[2]&&t[3])
	{
		error(line,col,"invalid type.");
	}
	if(t[0])
	{
		if(t[4])
		{
			return mkst("u8",0,l,c);
		}
		else
		{
			return mkst("s8",0,l,c);
		}
	}
	else if(t[0]+t[1]+t[2]+t[3]+t[4]+t[5])
	{
		if(t[2])
		{
			if(t[4])
			{
				return mkst("u16",0,l,c);
			}
			else
			{
				return mkst("s16",0,l,c);
			}
		}
		else if(t[3])
		{
			if(t[4])
			{
				return mkst("u64",0,l,c);
			}
			else
			{
				return mkst("s64",0,l,c);
			}
		}
		else
		{
			if(t[4])
			{
				return mkst("u32",0,l,c);
			}
			else
			{
				return mkst("s32",0,l,c);
			}
		}
	}
	else
	{
		return 0;
	}
}
struct syntax_tree *parse_struct_union_type(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node;
	oldword=current;
	if(!strcmp(cstr,"struct"))
	{
		ret=mkst("struct",0,line,col);
		next();
	}
	else if(!strcmp(cstr,"union"))
	{
		ret=mkst("union",0,line,col);
		next();
	}
	else
	{
		return 0;
	}
	node=parse_id_null();
	st_add_subtree(ret,node);
	if(strcmp(cstr,"{"))
	{
		return ret;
	}
	next();
	while(node=parse_type())
	{
		st_add_subtree(ret,node);
		node=parse_decl();
		if(node==0)
		{
			error(line,col,"invalid declaration.\n");
		}
		st_add_subtree(ret,node);
		if(strcmp(cstr,";"))
		{
			error(line,col,"expected \';\' after declaration.\n");
		}
		next();
	}
	if(strcmp(cstr,"}"))
	{
		error(line,col,"expected \'}\' after member list.\n");
	}
	next();
	return ret;
}
struct syntax_tree *parse_type(void)
{
	struct syntax_tree *ret;
	if(ret=parse_struct_union_type())
	{
		return ret;
	}
	return parse_basic_type();
}
struct syntax_tree *parse_decl(void);
struct syntax_tree *parse_decl_array(void)
{
	struct syntax_tree *ret,*node;
	long int l,c;
	l=line;
	c=col;
	if(strcmp(cstr,"["))
	{
		return 0;
	}
	next();
	if(!strcmp(cstr,"]"))
	{
		next();
		ret=mkst("array_nosize",0,l,c);
		st_add_subtree(ret,0);
		return ret;
	}
	if((node=parse_expr_15())==0)
	{
		error(line,col,"expected \']\' or expression after \'[\'.");
	}
	if(strcmp(cstr,"]"))
	{
		error(line,col,"expected \']\' after expression.");
	}
	next();
	ret=mkst("array",0,l,c);
	st_add_subtree(ret,0);
	st_add_subtree(ret,node);
	return ret;
}
struct syntax_tree *parse_decl_arglist(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node;
	oldword=current;
	ret=mkst("function",0,line,col);
	st_add_subtree(ret,0);
	if(!strcmp(cstr,")"))
	{
		return ret;
	}
	else if(!strcmp(cstr,"void"))
	{
		next();
		if(!strcmp(cstr,")"))
		{
			return ret;
		}
		resume();
	}
	while(1)
	{
		node=parse_type();
		if(!node)
		{
			error(line,col,"invalid declaration type.");
		}
		st_add_subtree(ret,node);
		node=parse_decl();
		if(!node)
		{
			error(line,col,"invalid declaration.");
		}
		st_add_subtree(ret,node);
		if(strcmp(cstr,","))
		{
			break;
		}
		next();
	}
	return ret;
}
struct syntax_tree *parse_decl_function(void)
{
	struct syntax_tree *ret;
	if(strcmp(cstr,"("))
	{
		return 0;
	}
	next();
	ret=parse_decl_arglist();
	if(strcmp(cstr,")"))
	{
		error(line,col,"expected \')\' after argument list.");
	}
	next();
	return ret;
}
struct syntax_tree *parse_decl_pointer(void)
{
	struct l_word_list *oldword;
	struct syntax_tree *ret,*node;
	oldword=current;
	if(strcmp(cstr,"*"))
	{
		return 0;
	}
	next();
	node=parse_decl();
	if(node==0)
	{
		resume();
		return 0;
	}
	ret=mkst("pointer",0,line,col);
	st_add_subtree(ret,node);
	return ret;
}
struct syntax_tree *parse_decl(void)
{
	struct syntax_tree *ret,*node;
	struct l_word_list *oldword;
	oldword=current;
	if(ret=parse_decl_pointer())
	{
		return ret;
	}
	if(!strcmp(cstr,"("))
	{
		next();
		ret=parse_decl();
		if(ret==0)
		{
			resume();
			return 0;
		}
		if(strcmp(cstr,")"))
		{
			error(line,col,"expected \')\' after \'(\'.");
		}
		next();
	}
	else
	{
		ret=parse_id_null();
		if(ret==0)
		{
			resume();
			return 0;
		}
	}
	while(1)
	{
		if(node=parse_decl_array())
		{
			node->subtrees[0]=ret;
			ret=node;
		}
		else if(node=parse_decl_function())
		{
			node->subtrees[0]=ret;
			ret=node;
		}
		else
		{
			break;
		}
	}
	return ret;
}
void type_global_init(void)
{
	basic_type_str[0]="char";
	basic_type_str[1]="int";
	basic_type_str[2]="short";
	basic_type_str[3]="long";
	basic_type_str[4]="unsigned";
	basic_type_str[5]="signed";
}
