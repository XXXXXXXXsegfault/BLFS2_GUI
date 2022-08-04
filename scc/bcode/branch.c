void gen_branch(struct ins *ins,char *op_1,char *op_2,char *op_3,char *op_4)
{
	struct operand op1,op2;
	int class1,class2;
	int sign,c;
	c=0;
	if(ins->count_args<4)
	{
		error(ins->line,"too few arguments.");
	}
	get_operand(ins,1,&op1);
	get_operand(ins,2,&op2);
	class1=0;
	class2=0;
	sign=1;
	if(op_is_reg(&op1))
	{
		if(!if_class_signed(op1.tab->class))
		{
			sign=0;
		}
		class1=1;
	}
	else if(op_is_const(&op1))
	{
		class1=2;
	}
	else if(op_is_addr(&op1))
	{
		class1=3;
		c=8;
	}
	else if(!if_class_signed(op1.tab->class))
	{
		sign=0;
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
		c=8;
	}
	else if(!if_class_signed(op2.tab->class))
	{
		sign=0;
	}
	if(class1==0||class1==1)
	{
		if(class2==0||class2==1)
		{
			if(op1.tab->class>op2.tab->class)
			{
				c=op1.tab->class;
			}
			else
			{
				c=op2.tab->class;
			}
		}
		else
		{
			c=op1.tab->class;
		}
	}
	else
	{
		if(class2==0||class2==1)
		{
			c=op2.tab->class;
		}
		else
		{
			c=8;
		}
	}
	/*
	if(class1==1)
	{
		if(class2==1)
		{
			reg_extend(c,op1.tab->class,&op1);
			reg_extend(c,op2.tab->class,&op2);
			outs("cmp ");
			op_out_reg(c,&op2);
			outs(",");
			op_out_reg(c,&op1);
			outs("\n");
			if(sign)
			{
				outs(op_1);
			}
			else
			{
				outs(op_2);
			}
			outs(" @_$LB");
			outs(ins->args[3]);
			outs("\n");
			last_store_valid=0;
			return;
		}
		else if(class2==0)
		{
			reg_extend(c,op1.tab->class,&op1);
			outs("cmp ");
			op_out_mem(&op2);
			outs(",");
			op_out_reg(c,&op1);
			outs("\n");
			if(sign)
			{
				outs(op_1);
			}
			else
			{
				outs(op_2);
			}
			outs(" @_$LB");
			outs(ins->args[3]);
			outs("\n");
			last_store_valid=0;
			return;
		}
		else if(class2==2&&op2.type==2&&(c<7||op2.value<=0x7fffffff||op2.value>=0xffffffff80000000))
		{
			if(op2.value==0&&(!strcmp(op_1,"je")||!strcmp(op_1,"jne")))
			{
				out_ins("test",0,0,&op1,&op1,0,op1.tab->class);
				outs(op_1);
				outs(" @_$LB");
				outs(ins->args[3]);
				outs("\n");
				last_store_valid=0;
				return;
			}
			reg_extend(c,op1.tab->class,&op1);
			outs("cmp $");
			op_out_const(c,&op2);
			outs(",");
			op_out_reg(c,&op1);
			outs("\n");
			if(sign)
			{
				outs(op_1);
			}
			else
			{
				outs(op_2);
			}
			outs(" @_$LB");
			outs(ins->args[3]);
			outs("\n");
			last_store_valid=0;
			return;
		}
	}
	else if(class1==0)
	{
		if(class2==1)
		{
			out_ins_acd1("mov",0,0,&op1,0,c);
			out_ins_acd1("cmp",0,0,&op2,0,c);
			if(sign)
			{
				outs(op_1);
			}
			else
			{
				outs(op_2);
			}
			outs(" @_$LB");
			outs(ins->args[3]);
			outs("\n");
			last_store_valid=0;
			return;
		}
		else if(class2==2&&op2.type==2&&(c<7||op2.value<=0x7fffffff||op2.value>=0xffffffff80000000))
		{
			out_ins("cmp",get_len(c),0,&op2,&op1,0,c);
			if(sign)
			{
				outs(op_1);
			}
			else
			{
				outs(op_2);
			}
			outs(" @_$LB");
			outs(ins->args[3]);
			outs("\n");
			last_store_valid=0;
			return;
		}
		else if(class2==0)
		{
			out_ins_acd1("mov",0,0,&op2,0,c);
			out_ins_acd2("cmp",0,0,0,&op1,c);
			if(sign)
			{
				outs(op_1);
			}
			else
			{
				outs(op_2);
			}
			outs(" @_$LB");
			outs(ins->args[3]);
			outs("\n");
			last_store_valid=0;
			return;
		}
	}
	*/
	if(class1==1)
	{
		outs("mov ");
		op_out_reg(op1.tab->class,&op1);
		outs(",");
		out_rax(op1.tab->class);
		outs("\n");
		acd_extend(0,c,op1.tab->class);
	}
	else if(class1==2)
	{
		outs("mov $");
		op_out_const(c,&op1);
		outs(",");
		out_rax(c);
		outs("\n");
	}
	else if(class1==0)
	{
		outs("mov ");
		op_out_mem(&op1);
		outs(",");
		out_rax(op1.tab->class);
		outs("\n");
		acd_extend(0,c,op1.tab->class);
	}
	else
	{
		outs("lea ");
		op_out_mem(&op1);
		outs(",");
		out_rax(8);
		outs("\n");
	}
	if(class2==1)
	{
		outs("mov ");
		op_out_reg(op2.tab->class,&op2);
		outs(",");
		out_rcx(op2.tab->class);
		outs("\n");
		acd_extend(1,c,op2.tab->class);
	}
	else if(class2==2)
	{
		outs("mov $");
		op_out_const(c,&op2);
		outs(",");
		out_rcx(c);
		outs("\n");
	}
	else if(class2==0)
	{
		outs("mov ");
		op_out_mem(&op2);
		outs(",");
		out_rcx(op2.tab->class);
		outs("\n");
		acd_extend(1,c,op2.tab->class);
	}
	else
	{
		outs("lea ");
		op_out_mem(&op2);
		outs(",");
		out_rcx(8);
		outs("\n");
	}
	outs("cmp ");
	out_rcx(c);
	outs(",");
	out_rax(c);
	outs("\n");
	if(sign)
	{
		outs(op_1);
	}
	else
	{
		outs(op_2);
	}
	outs(" @_$LB");
	outs(ins->args[3]);
	outs("\n");
	last_store_valid=0;
}
