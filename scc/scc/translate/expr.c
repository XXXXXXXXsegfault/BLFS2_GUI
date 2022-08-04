long int new_tmp_name;
char *mktmpname(void)
{
	char *ret;
	++new_tmp_name;
	ret=xstrdup("_$T$");
	ret=str_i_app(ret,new_tmp_name);
	return ret;
}
unsigned long int const_to_num(char *str)
{
	unsigned long int ret;
	int x;
	ret=0;
	if(str[0]=='\'')
	{
		++str;
		if(str[0]=='\\')
		{
			if(str[1]=='\\')
			{
				ret='\\';
			}
			else if(str[1]=='n')
			{
				ret='\n';
			}
			else if(str[1]=='t')
			{
				ret='\t';
			}
			else if(str[1]=='v')
			{
				ret='\v';
			}
			else if(str[1]=='r')
			{
				ret='\r';
			}
			else if(str[1]=='\'')
			{
				ret='\'';
			}
			else if(str[1]=='\"')
			{
				ret='\"';
			}
			else if(str[1]=='\?')
			{
				ret='\?';
			}
			else if(str[1]>='0'&&str[1]<='7')
			{
				x=1;
				while(str[x]>='0'&&str[x]<='7')
				{
					ret=(ret<<3)+(str[x]-'0');
					++x;
				}
			}
			else if(str[1]=='x')
			{
				x=2;
				while(1)
				{
					if(str[x]>='0'&&str[x]<='9')
					{
						ret=ret*16+(str[x]-'0');
					}
					else if(str[x]>='A'&&str[x]<='F')
					{
						ret=ret*16+(str[x]-'A'+10);
					}
					else if(str[x]>='a'&&str[x]<='f')
					{
						ret=ret*16+(str[x]-'a'+10);
					}
					else
					{
						break;
					}
					++x;
				}
			}
			else
			{
				ret='\\';
			}
		}
		else
		{
			ret=str[0];
		}
	}
	else if(str[0]>='1'&&str[0]<='9')
	{
		x=0;
		while(str[x]>='0'&&str[x]<='9')
		{
			ret=ret*10+(str[x]-'0');
			++x;
		}
	}
	else if(str[1]=='X'||str[1]=='x')
	{
		x=2;
		while(1)
		{
			if(str[x]>='0'&&str[x]<='9')
			{
				ret=ret*16+(str[x]-'0');
			}
			else if(str[x]>='A'&&str[x]<='F')
			{
				ret=ret*16+(str[x]-'A'+10);
			}
			else if(str[x]>='a'&&str[x]<='f')
			{
				ret=ret*16+(str[x]-'a'+10);
			}
			else
			{
				break;
			}
			++x;
		}
	}
	else
	{
		x=0;
		while(str[x]>='0'&&str[x]<='7')
		{
			ret=(ret<<3)+(str[x]-'0');
			++x;
		}
	}
	return ret;
}
struct syntax_tree *get_addr(struct syntax_tree *decl)
{
	struct syntax_tree *decl1,*ret,*node;
	ret=syntax_tree_dup(decl);
	decl1=get_decl_type(ret);
	node=mkst("pointer",0,decl1->line,decl1->col);
	if(!strcmp(decl1->name,"Identifier"))
	{
		st_add_subtree(node,decl1);
		return node;
	}
	st_add_subtree(node,decl1->subtrees[0]);
	decl1->subtrees[0]=node;
	return ret;
}
void calculate_id(struct syntax_tree *root,struct expr_ret *ret)
{
	struct id_tab *id;
	struct syntax_tree *decl;
	id=id_find(root->value);
	if(!id)
	{
		error(root->line,root->col,"identifier not declared.");
	}
	decl=get_decl_type(id->decl);
	ret->is_lval=1;
	if(!strcmp(decl->name,"function"))
	{
		ret->is_lval=0;
	}

	ret->is_const=0;
	ret->needs_deref=0;
	ret->type=syntax_tree_dup(id->type);
	ret->decl=syntax_tree_dup(id->decl);
}
void calculate_const(struct syntax_tree *root,struct expr_ret *ret)
{
	char *t_name;
	struct syntax_tree *type,*decl,*node;
	if(root->value[0]=='\"')
	{
		t_name=mktmpname();
		type=mkst("s8",0,root->line,root->col);
		node=mkst("Identifier",t_name,root->line,root->col);
		decl=mkst("pointer",0,root->line,root->col);
		st_add_subtree(decl,node);
		ret->type=type;
		ret->decl=decl;
		ret->is_lval=0;
		ret->is_const=0;
		ret->needs_deref=0;
		c_write("local u64 ",10);
		c_write(t_name,strlen(t_name));
		c_write("\n",1);
		c_write("mov ",4);
		c_write(t_name,strlen(t_name));
		c_write(" ",1);
		c_write(root->value,strlen(root->value));
		c_write("\n",1);
	}
	else
	{
		ret->is_lval=0;
		ret->is_const=1;
		ret->needs_deref=0;
		ret->type=mkst("u64",0,root->line,root->col);
		ret->decl=mkst("Identifier","<NULL>",root->line,root->col);
		ret->value=const_to_num(root->value);
	}
}
void deref_ptr(struct expr_ret *ret,int line,int col)
{
	char *str,*old_name;
	struct syntax_tree *decl,*t;
	char *size;
	int s;
	s=1;
	if(!ret->needs_deref)
	{
		return;
	}
	str=mktmpname();
	decl=decl_next(ret->decl);
	syntax_tree_release(ret->decl);
	ret->decl=decl;
	t=get_decl_type(ret->decl);
	if(!strcmp(t->name,"pointer"))
	{
		old_name=t->subtrees[0]->value;
		t->subtrees[0]->value=str;
		size="q ";
	}
	else if(!strcmp(t->name,"Identifier"))
	{
		if(!strcmp(ret->type->name,"struct"))
		{
			error(line,col,"invalid use of structure.");
		}
		if(!strcmp(ret->type->name,"union"))
		{
			error(line,col,"invalid use of union.");
		}
		if(!strcmp(ret->type->name,"void"))
		{
			error(line,col,"invalid type.");
		}
		if(!strcmp(ret->type->name,"s8")||!strcmp(ret->type->name,"u8"))
		{
			size="b ";
		}
		else if(!strcmp(ret->type->name,"s16")||!strcmp(ret->type->name,"u16"))
		{
			size="w ";
		}
		else if(!strcmp(ret->type->name,"s32")||!strcmp(ret->type->name,"u32"))
		{
			size="l ";
		}
		else if(!strcmp(ret->type->name,"s64")||!strcmp(ret->type->name,"u64"))
		{
			size="q ";
		}
		old_name=t->value;
		t->value=str;
	}
	else
	{
		s=0;
		array_function_to_pointer2(&ret->decl);
	}
	if(s)
	{
		add_decl(ret->type,ret->decl,0,0,0,1);
		c_write("ld",2);
		c_write(size,2);
		c_write(str,strlen(str));
		c_write(" ",1);
		c_write(old_name,strlen(old_name));
		c_write("\n",1);
		free(old_name);
	}
}
void scale_result(struct syntax_tree *type,struct syntax_tree *decl,long int scale)
{
	char *str,*old_str;
	struct syntax_tree *decl1;
	if(scale!=1)
	{
		str=mktmpname();
		decl1=get_decl_type(decl);
		if(!strcmp(decl1->name,"Identifier"))
		{
			old_str=decl1->value;
			decl1->value=str;
		}
		else
		{
			old_str=decl1->subtrees[0]->value;
			decl1->subtrees[0]->value=str;
		}
		add_decl(type,decl,0,0,0,1);
		c_write("mul ",4);
		c_write(str,strlen(str));
		c_write(" ",1);
		c_write(old_str,strlen(old_str));
		c_write(" ",1);
		free(old_str);
		old_str=str_i_app(0,scale);
		c_write(old_str,strlen(old_str));
		c_write("\n",1);
		free(old_str);
	}
}
void r_scale_result(struct syntax_tree *type,struct syntax_tree *decl,long int scale)
{
	char *str,*old_str;
	struct syntax_tree *decl1;
	if(scale!=1)
	{
		str=mktmpname();
		decl1=get_decl_type(decl);
		if(!strcmp(decl1->name,"Identifier"))
		{
			old_str=decl1->value;
			decl1->value=str;
		}
		else
		{
			old_str=decl1->subtrees[0]->value;
			decl1->subtrees[0]->value=str;
		}
		add_decl(type,decl,0,0,0,1);
		c_write("div ",4);
		c_write(str,strlen(str));
		c_write(" ",1);
		c_write(old_str,strlen(old_str));
		c_write(" ",1);
		free(old_str);
		old_str=str_i_app(0,scale);
		c_write(old_str,strlen(old_str));
		c_write("\n",1);
		free(old_str);
	}
}
#include "branch.c"
#include "assign.c"
#include "add.c"
#include "mul.c"
#include "sub.c"
#include "div.c"
#include "mod.c"
#include "and.c"
#include "orr.c"
#include "eor.c"
#include "lsh.c"
#include "rsh.c"
#include "neg.c"
#include "not.c"
#include "member.c"
#include "sizeof.c"
#include "ptr.c"
#include "inc.c"
#include "dec.c"
#include "assign_add.c"
#include "assign_sub.c"
#include "assign_op.c"
#include "control.c"
#include "call.c"
#include "cast.c"
void calculate_expr(struct syntax_tree *root,struct expr_ret *ret)
{
	struct expr_ret left;
	memset(ret,0,sizeof(*ret));
	if(!strcmp(root->name,"Identifier"))
	{
		calculate_id(root,ret);
	}
	else if(!strcmp(root->name,"Constant"))
	{
		calculate_const(root,ret);
	}
	else if(!strcmp(root->name,","))
	{
		calculate_expr(root->subtrees[0],&left);
		calculate_expr(root->subtrees[1],ret);
		expr_ret_release(&left);
	}
	else if(!strcmp(root->name,"="))
	{
		calculate_assign(root,ret,"mov","st");
	}
	else if(!strcmp(root->name,"+"))
	{
		calculate_add(root,ret);
	}
	else if(!strcmp(root->name,"*"))
	{
		calculate_mul(root,ret);
	}
	else if(!strcmp(root->name,"-"))
	{
		calculate_sub(root,ret);
	}
	else if(!strcmp(root->name,"/"))
	{
		calculate_div(root,ret);
	}
	else if(!strcmp(root->name,"%"))
	{
		calculate_mod(root,ret);
	}
	else if(!strcmp(root->name,"&"))
	{
		calculate_and(root,ret);
	}
	else if(!strcmp(root->name,"|"))
	{
		calculate_orr(root,ret);
	}
	else if(!strcmp(root->name,"^"))
	{
		calculate_eor(root,ret);
	}
	else if(!strcmp(root->name,"<<"))
	{
		calculate_lsh(root,ret);
	}
	else if(!strcmp(root->name,">>"))
	{
		calculate_rsh(root,ret);
	}
	else if(!strcmp(root->name,"neg"))
	{
		calculate_neg(root,ret);
	}
	else if(!strcmp(root->name,"~"))
	{
		calculate_not(root,ret);
	}
	else if(!strcmp(root->name,"."))
	{
		calculate_member(root,ret);
	}
	else if(!strcmp(root->name,"->"))
	{
		calculate_member_ptr(root,ret);
	}
	else if(!strcmp(root->name,"sizeof"))
	{
		calculate_sizeof(root,ret);
	}
	else if(!strcmp(root->name,"sizeof_type"))
	{
		calculate_sizeof_type(root,ret);
	}
	else if(!strcmp(root->name,"addr"))
	{
		calculate_addr(root,ret);
	}
	else if(!strcmp(root->name,"deref"))
	{
		calculate_deref(root,ret);
	}
	else if(!strcmp(root->name,"[]"))
	{
		calculate_index(root,ret);
	}
	else if(!strcmp(root->name,"++"))
	{
		calculate_inc(root,ret);
	}
	else if(!strcmp(root->name,"--"))
	{
		calculate_dec(root,ret);
	}
	else if(!strcmp(root->name,"+="))
	{
		calculate_assign_add(root,ret);
	}
	else if(!strcmp(root->name,"-="))
	{
		calculate_assign_sub(root,ret);
	}
	else if(!strcmp(root->name,"*="))
	{
		calculate_assign_op(root,ret,"*=","mul");
	}
	else if(!strcmp(root->name,"/="))
	{
		calculate_assign_op(root,ret,"/=","div");
	}
	else if(!strcmp(root->name,"%="))
	{
		calculate_assign_op(root,ret,"%=","mod");
	}
	else if(!strcmp(root->name,"<<="))
	{
		calculate_assign_op(root,ret,"<<=","lsh");
	}
	else if(!strcmp(root->name,">>="))
	{
		calculate_assign_op(root,ret,">>=","rsh");
	}
	else if(!strcmp(root->name,"&="))
	{
		calculate_assign_op(root,ret,"&=","and");
	}
	else if(!strcmp(root->name,"|="))
	{
		calculate_assign_op(root,ret,"|=","orr");
	}
	else if(!strcmp(root->name,"^="))
	{
		calculate_assign_op(root,ret,"^=","eor");
	}
	else if(!strcmp(root->name,">"))
	{
		calculate_branch(root,ret);
	}
	else if(!strcmp(root->name,"<"))
	{
		calculate_branch(root,ret);
	}
	else if(!strcmp(root->name,">="))
	{
		calculate_branch(root,ret);
	}
	else if(!strcmp(root->name,"<="))
	{
		calculate_branch(root,ret);
	}
	else if(!strcmp(root->name,"=="))
	{
		calculate_branch(root,ret);
	}
	else if(!strcmp(root->name,"!="))
	{
		calculate_branch(root,ret);
	}
	else if(!strcmp(root->name,"!"))
	{
		calculate_branch(root,ret);
	}
	else if(!strcmp(root->name,"&&"))
	{
		calculate_branch(root,ret);
	}
	else if(!strcmp(root->name,"||"))
	{
		calculate_branch(root,ret);
	}
	else if(!strcmp(root->name,"call"))
	{
		calculate_call(root,ret);
	}
	else if(!strcmp(root->name,"call_noarg"))
	{
		calculate_call(root,ret);
	}
	else if(!strcmp(root->name,"cast"))
	{
		calculate_cast(root,ret);
	}
	else
	{
		error(root->line,root->col,"unsupported operator.");
	}
}
