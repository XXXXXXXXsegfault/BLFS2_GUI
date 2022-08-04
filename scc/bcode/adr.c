
void gen_adr(struct ins *ins)
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
	if(class2==1||class2==2)
	{
		error(ins->line,"invalid op.");
	}
	if(class1==1)
	{
		outs("lea ");
		op_out_mem(&op2);
		outs(",");
		op_out_reg(8,&op1);
		outs("\n");
	}
	else if(class1==0)
	{
		outs("lea ");
		op_out_mem(&op2);
		outs(",");
		out_rax(8);
		outs("\n");
		outs("mov ");
		out_rax(op1.tab->class);
		outs(",");
		op_out_mem(&op1);
		outs("\n");
	}
	else
	{
		error(ins->line,"invalid op.");
	}
}
