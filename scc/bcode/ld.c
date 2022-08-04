void gen_ld(struct ins *ins,int c)
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
	if(class1==2||class1==3||class2==3)
	{
		error(ins->line,"invalid op.");
	}
	if(class1==1)
	{
		if(class2==1)
		{
			reg_extend(8,op2.tab->class,&op2);
			outs("mov (");
			op_out_reg(8,&op2);
			outs("),");
			op_out_reg(c,&op1);
			outs("\n");
			reg_extend(op1.tab->class,c,&op1);
		}
		else
		{
			if(class2==2)
			{
				outs("mov $");
				op_out_const(8,&op2);
				outs(",");
				out_rax(8);
				outs("\n");
			}
			else if(class2==0)
			{
				outs("mov ");
				op_out_mem(&op2);
				outs(",");
				out_rax(op2.tab->class);
				outs("\n");
				acd_extend(0,8,op2.tab->class);
			}
			outs("mov (%rax),");
			op_out_reg(c,&op1);
			outs("\n");
			reg_extend(op1.tab->class,c,&op1);
		}
	}
	else if(class1==0)
	{
		if(class2==1)
		{
			reg_extend(8,op2.tab->class,&op2);
			outs("mov (");
			op_out_reg(8,&op2);
			outs("),");
			out_rax(c);
			outs("\n");

			acd_extend(0,op1.tab->class,c);
			outs("mov ");
			out_rax(op1.tab->class);
			outs(",");
			op_out_mem(&op1);
			outs("\n");
		}
		else
		{
			if(class2==2)
			{
				outs("mov $");
				op_out_const(8,&op2);
				outs(",");
				out_rax(8);
				outs("\n");
			}
			else if(class2==0)
			{
				outs("mov ");
				op_out_mem(&op2);
				outs(",");
				out_rax(op2.tab->class);
				outs("\n");
				acd_extend(0,8,op2.tab->class);
			}
			outs("mov (%rax),");
			out_rax(c);
			outs("\n");
			acd_extend(0,op1.tab->class,c);
			outs("mov ");
			out_rax(op1.tab->class);
			outs(",");
			op_out_mem(&op1);
			outs("\n");
		}
	}
}
void gen_ldo(struct ins *ins,int c)
{
	struct operand op1,op2,op3;
	int class1,class2,class3;
	get_operand(ins,1,&op1);
	get_operand(ins,2,&op2);
	get_operand(ins,3,&op3);
	class1=0;
	class2=0;
	class3=0;
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
	if(op_is_reg(&op3))
	{
		class3=1;
	}
	else if(op_is_const(&op3))
	{
		class3=2;
	}
	else if(op_is_addr(&op3))
	{
		class3=3;
	}
	if(class1==2||class1==3)
	{
		error(ins->line,"invalid op.");
	}
	if(class2==1)
	{
		if(class3==1)
		{
			if(class1==1)
			{
				reg_extend(7,op2.tab->class,&op2);
				reg_extend(7,op3.tab->class,&op3);
				outs("mov (");
				op_out_reg(8,&op2);
				outs(",");
				op_out_reg(8,&op3);
				outs("),");
				op_out_reg(c,&op1);
				outs("\n");
				return;
			}
			else if(class1==0)
			{
				reg_extend(7,op2.tab->class,&op2);
				reg_extend(7,op3.tab->class,&op3);
				outs("mov (");
				op_out_reg(8,&op2);
				outs(",");
				op_out_reg(8,&op3);
				outs("),");
				out_rax(c);
				outs("\n");
				out_ins_acd2("mov",0,0,0,&op1,c);
				return;
			}
		}
		else if(class3==2&&op3.type==2&&(op3.value<=0x7fffff||op3.value>=0xffffffffff800000))
		{
			if(class1==1)
			{
				reg_extend(7,op2.tab->class,&op2);
				outs("mov ");
				op_out_const(8,&op3);
				outs("(");
				op_out_reg(8,&op2);
				outs("),");
				op_out_reg(c,&op1);
				outs("\n");
				return;
			}
			else if(class1==0)
			{
				reg_extend(7,op2.tab->class,&op2);
				outs("mov ");
				op_out_const(8,&op3);
				outs("(");
				op_out_reg(8,&op2);
				outs("),");
				out_rax(c);
				outs("\n");
				out_ins_acd2("mov",0,0,0,&op1,c);
				return;
			}
		}
	}
	if(class2==3)
	{
		if(class3==1||class3==2&&op3.type==2&&(op3.value<=0x7fffff||op3.value>=0xffffffffff800000))
		{
			if(class3==1)
			{
				reg_extend(7,op3.tab->class,&op3);
			}
			outs("mov ");
			op_out_mem_off(&op2,&op3);
			outs(",");
			if(class1==1)
			{
				op_out_reg(c,&op1);
				outs("\n");
			}
			else
			{
				out_rax(c);
				outs("\n");
				out_ins_acd2("mov",0,0,0,&op1,c);
			}
			return;
		}
	}
	if(class2==3)
	{
		out_ins_acd1("lea",0,0,&op2,0,8);
	}
	else
	{
		out_ins_acd1("mov",0,0,&op2,0,8);
	}
	if(class3==3)
	{
		out_ins_acd1("lea",0,0,&op3,1,8);
	}
	else
	{
		out_ins_acd1("mov",0,0,&op3,1,8);
	}
	outs("mov (%rax,%rcx),");
	if(class1==1)
	{
		op_out_reg(c,&op1);
		outs("\n");
	}
	else
	{
		out_rcx(c);
		outs("\n");
		outs("mov ");
		out_rcx(c);
		outs(",");
		op_out_mem(&op1);
		outs("\n");
	}
}
