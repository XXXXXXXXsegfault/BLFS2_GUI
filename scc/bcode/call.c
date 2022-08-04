void gen_push(struct ins *ins)
{
	struct operand op1;
	int class1;
	get_operand(ins,1,&op1);
	class1=0;
	if(op_is_reg(&op1))
	{
		class1=1;
	}
	else if(op_is_const(&op1))
	{
		class1=2;
	}
	else if(op_is_addr(&op1))
	{
		class1=3;
	}
	if(class1==2&&op1.type==2&&(op1.value<=0x7fffffff||op1.value>=0xffffffff80000000))
	{
		outs("pushq $");
		op_out_const(8,&op1);
		outs("\n");
		return;
	}
	if(class1==1)
	{
		reg_extend(7,op1.tab->class,&op1);
		outs("push ");
		op_out_reg(7,&op1);
		outs("\n");
	}
	else
	{
		if(class1==2)
		{
			outs("mov $");
			op_out_const(7,&op1);
			outs(",%rax\n");
		}
		else if(class1==0)
		{
			outs("mov ");
			op_out_mem(&op1);
			outs(",");
			out_rax(op1.tab->class);
			outs("\n");
			acd_extend(0,7,op1.tab->class);
		}
		else if(class1==3)
		{
			outs("lea ");
			op_out_mem(&op1);
			outs(",");
			out_rax(8);
			outs("\n");
		}
		outs("push %rax\n");
	}
}
void gen_call(struct ins *ins)
{
	struct operand op1,op2;
	int class1,class2;
	get_operand(ins,1,&op1);
	get_operand(ins,2,&op2);
	class1=0;
	class2=0;
	if(op_is_reg(&op1))
	{
		class1=1;
	}
	else if(op_is_const(&op1))
	{
		class1=2;
	}
	else if(op_is_addr(&op1))
	{
		class1=3;
	}
	if(op_is_reg(&op2))
	{
		class2=1;
	}
	else if(op_is_const(&op2))
	{
		class2=2;
	}
	else if(op_is_addr(&op2))
	{
		class2=3;
	}
	if(class1==2||class1==3)
	{
		error(ins->line,"invalid op.");
	}
	if(class2==2)
	{
		outs("call ");
		op_out_const(8,&op2);
		outs("\n");
	}
	else if(class2==1)
	{
		reg_extend(8,op2.tab->class,&op2);
		outs("call *");
		op_out_reg(8,&op2);
		outs("\n");
	}
	else
	{
		if(class2==0)
		{
			outs("mov ");
			op_out_mem(&op2);
			outs(",");
			out_rax(op2.tab->class);
			outs("\n");
			acd_extend(0,8,op2.tab->class);
		}
		else
		{
			outs("lea ");
			op_out_mem(&op2);
			outs(",");
			out_rax(8);
			outs("\n");
		}
		outs("call *%rax\n");
	}
	last_store_valid=0;
	if(op1.tab->unused)
	{
		return;
	}
	if(class1==1)
	{
		outs("mov ");
		out_rax(op1.tab->class);
		outs(",");
		op_out_reg(op1.tab->class,&op1);
		outs("\n");
	}
	else
	{
		outs("mov ");
		out_rax(op1.tab->class);
		outs(",");
		op_out_mem(&op1);
		outs("\n");
	}
}
void gen_retval(struct ins *ins)
{
	struct operand op1;
	int class1;
	struct ins *p;
	get_operand(ins,1,&op1);
	class1=0;
	if(fun_name==0)
	{
		error(ins->line,"ret outside of function.");
	}
	if(op_is_reg(&op1))
	{
		class1=1;
	}
	else if(op_is_const(&op1))
	{
		class1=2;
	}
	else if(op_is_addr(&op1))
	{
		class1=3;
	}
	if(class1==1)
	{
		reg_extend(7,op1.tab->class,&op1);
		outs("mov ");
		op_out_reg(7,&op1);
		outs(",%rax\n");
	}
	else
	{
		if(class1==2)
		{
			outs("mov $");
			op_out_const(7,&op1);
			outs(",%rax\n");
		}
		else if(class1==0)
		{
			outs("mov ");
			op_out_mem(&op1);
			outs(",");
			out_rax(op1.tab->class);
			outs("\n");
			acd_extend(0,7,op1.tab->class);
		}
		else if(class1==3)
		{
			outs("lea ");
			op_out_mem(&op1);
			outs(",");
			out_rax(8);
			outs("\n");
		}
	}
	p=ins->next;
	while(p)
	{
		if(p->op)
		{
			outs("jmp ");
			outs("@");
			outs(fun_name);
			outs("$END\n");
			break;
		}
		if(p->count_args&&!strcmp(p->args[0],"endf"))
		{
			break;
		}
		p=p->next;
	}
}
