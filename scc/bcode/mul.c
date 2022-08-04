void gen_mul(struct ins *ins)
{
	struct operand op1,op2,op3;
	int class1,class2,class3;
	int sign;
	unsigned long int x,n;
	get_operand(ins,1,&op1);
	get_operand(ins,2,&op2);
	get_operand(ins,3,&op3);
	class1=0;
	class2=0;
	class3=0;
	sign=1;
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
		if(!if_class_signed(op2.tab->class))
		{
			sign=0;
		}
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
	else if(!if_class_signed(op2.tab->class))
	{
		sign=0;
	}
	if(op_is_reg(&op3))
	{
		if(!if_class_signed(op3.tab->class))
		{
			sign=0;
		}
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
	else if(!if_class_signed(op3.tab->class))
	{
		sign=0;
	}
	if(class1==2||class1==3)
	{
		error(ins->line,"invalid op.");
	}
	if(class3==2&&op3.type==2)
	{
		if(class1==1&&class2==1)
		{
			n=2;
			x=1;
			if(op3.value==0)
			{
				out_ins("xor",0,0,&op1,&op1,0,op1.tab->class);
				return;
			}
			else if(op3.value==1)
			{
				if(opcmp(&op1,&op2))
				{
					out_ins("mov",0,0,&op2,&op1,0,op1.tab->class);
				}
				return;
			}
			else if(!opcmp(&op1,&op2))
			{
				while(x<64)
				{
					if(n==op3.value)
					{
						outs("shl $");
						out_num8(x);
						outs(",");
						op_out_reg(op1.tab->class,&op1);
						outs("\n");
						return;
					}
					n=n<<1;
					++x;
				}
			}
			else
			{
				out_ins("mov",0,0,&op2,&op1,0,op1.tab->class);
				while(x<64)
				{
					if(n==op3.value)
					{
						outs("shl $");
						out_num8(x);
						outs(",");
						op_out_reg(op1.tab->class,&op1);
						outs("\n");
						return;
					}
					n=n<<1;
					++x;
				}
			}
		}
	}
	if(class2==1)
	{
		outs("mov ");
		op_out_reg(op2.tab->class,&op2);
		outs(",");
		out_rax(op2.tab->class);
		outs("\n");
		acd_extend(0,op1.tab->class,op2.tab->class);
	}
	else if(class2==2)
	{
		outs("mov $");
		op_out_const(op1.tab->class,&op2);
		outs(",");
		out_rax(op1.tab->class);
		outs("\n");
	}
	else if(class2==0)
	{
		outs("mov ");
		op_out_mem(&op2);
		outs(",");
		out_rax(op2.tab->class);
		outs("\n");
		acd_extend(0,op1.tab->class,op2.tab->class);
	}
	else
	{
		outs("lea ");
		op_out_mem(&op2);
		outs(",");
		out_rax(8);
		outs("\n");
	}
	if(class3==1)
	{
		outs("mov ");
		op_out_reg(op3.tab->class,&op3);
		outs(",");
		out_rcx(op3.tab->class);
		outs("\n");
		acd_extend(1,op1.tab->class,op3.tab->class);
	}
	else if(class3==2)
	{
		outs("mov $");
		op_out_const(op1.tab->class,&op3);
		outs(",");
		out_rcx(op1.tab->class);
		outs("\n");
	}
	else if(class3==0)
	{
		outs("mov ");
		op_out_mem(&op3);
		outs(",");
		out_rcx(op3.tab->class);
		outs("\n");
		acd_extend(1,op1.tab->class,op3.tab->class);
	}
	else
	{
		outs("lea ");
		op_out_mem(&op3);
		outs(",");
		out_rcx(8);
		outs("\n");
	}
	if(sign)
	{
		outs("i");
	}
	outs("mul ");
	out_rcx(op1.tab->class);
	outs("\n");

	if(class1==1)
	{
		outs("mov ");
		out_rax(op1.tab->class);
		outs(",");
		op_out_reg(op1.tab->class,&op1);
		outs("\n");
	}
	else if(class1==0)
	{
		outs("mov ");
		out_rax(op1.tab->class);
		outs(",");
		op_out_mem(&op1);
		outs("\n");
	}
}
