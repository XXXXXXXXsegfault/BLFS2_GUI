void write_addr_prefix(struct addr *addr)
{
	if(addr->bit32)
	{
		soutc(0x67);
	}
}
unsigned char get_addr_rex(struct addr *addr)
{
	unsigned char rex;
	rex=0;
	if(addr->reg1>=8&&addr->reg1<16)
	{
		rex|=0x41;
	}
	if(addr->reg2>=8&&addr->reg1<16)
	{
		rex|=0x42;
	}
	return rex;
}
void write_addr(int modrm,struct addr *addr)
{
	int reg,reg2;
	if(addr->reg1<0)
	{
		if(addr->reg2<0)
		{
			soutc(modrm|0x04);
			soutc(0x25);
			swrite(&addr->offset,4);
		}
		else
		{
			soutc(modrm|0x04);
			soutc(0x05|addr->scale);
			swrite(&addr->offset,4);
		}
	}
	else if(addr->reg1==16)
	{
		soutc(modrm|0x05);
		swrite(&addr->offset,4);
	}
	else if(addr->reg2<0)
	{
		reg=addr->reg1&7;
		if(addr->offset==0&&reg!=5)
		{
			if(reg==4)
			{
				soutc(modrm|0x04);
				soutc(0x24);
			}
			else
			{
				soutc(modrm|reg);
			}
		}
		else if(addr->offset<=0x7f||addr->offset>=0xffffff80)
		{
			soutc(modrm|0x40|reg);
			if(reg==4)
			{
				soutc(0x24);
			}
			swrite(&addr->offset,1);
		}
		else
		{
			soutc(modrm|0x80|reg);
			if(reg==4)
			{
				soutc(0x24);
			}
			swrite(&addr->offset,4);
		}
	}
	else
	{
		reg=addr->reg1&7;
		reg2=addr->reg2&7;
		if(addr->offset==0&&reg!=5)
		{
			soutc(modrm|0x04);
			soutc(addr->scale|reg2<<3|reg);
		}
		else if(addr->offset<=0x7f||addr->offset>=0xffffff80)
		{
			soutc(modrm|0x44);
			soutc(addr->scale|reg2<<3|reg);
			swrite(&addr->offset,1);
		}
		else
		{
			soutc(modrm|0x84);
			soutc(addr->scale|reg2<<3|reg);
			swrite(&addr->offset,4);
		}
	}
}
int write_default_ins(struct ins *ins,struct ins_args *args)
{
	int rex,modrm;
	int s;
	rex=ins->rex;
	modrm=ins->modrm;
	if(ins->flags&I_IMM)
	{
		s=ins->flags&I_IMMMASK;
		if(s==I_IMMB)
		{
			if(ins->flags&I_U)
			{
				if(args->imm>0xff&&args->imm<0xffffffffffffff80)
				{
					return -1;
				}
			}
			else
			{
				if(args->imm>0x7f&&args->imm<0xffffffffffffff80)
				{
					return -1;
				}
			}
		}
		else if(s==I_IMMW)
		{
			if(ins->flags&I_U)
			{
				if(args->imm>0xffff&&args->imm<0xffffffffffff8000)
				{
					return -1;
				}
			}
			else
			{
				if(args->imm>0x7fff&&args->imm<0xffffffffffff8000)
				{
					return -1;
				}
			}
		}
		else if(s==I_IMML)
		{
			if(ins->flags&I_U)
			{
				if(args->imm>0xffffffff&&args->imm<0xffffffff80000000)
				{
					return -1;
				}
			}
			else
			{
				if(args->imm>0x7fffffff&&args->imm<0xffffffff80000000)
				{
					return -1;
				}
			}
		}
	}
	if(ins->flags&I_MEM)
	{
		write_addr_prefix(&args->addr);
		rex|=get_addr_rex(&args->addr);
	}
	if(ins->flags&I_REG1)
	{
		if(args->reg1>=8&&args->reg1<16)
		{
			rex|=0x41;
		}
		if(args->reg1>=20&&args->reg1<24)
		{
			rex|=0x40;
		}
		modrm|=args->reg1&7;
	}
	if(ins->flags&I_REG2)
	{
		if(args->reg2>=8&&args->reg2<16)
		{
			rex|=0x44;
		}
		if(args->reg2>=20&&args->reg2<24)
		{
			rex|=0x40;
		}
		modrm|=(args->reg2&7)<<3;
	}
	if(ins->prefix)
	{
		swrite(ins->prefix,strlen(ins->prefix));
	}
	if(rex)
	{
		if(ins->flags&I_REG1)
		{
			if(args->reg1>=28&&args->reg1<32)
			{
				error(l->line,"invalid register with REX.");
			}
		}
		if(ins->flags&I_REG2)
		{
			if(args->reg2>=28&&args->reg2<32)
			{
				error(l->line,"invalid register with REX.");
			}
		}
		soutc(rex);
	}
	swrite(ins->opcode,ins->opcode_len);
	if(ins->flags&I_MEM)
	{
		write_addr(modrm,&args->addr);
	}
	else if(ins->flags&I_MODRM)
	{
		soutc(modrm|0xc0);
	}
	if(ins->flags&I_IMM)
	{
		s=ins->flags&I_IMMMASK;
		if(s==I_IMMB)
		{
			swrite(&args->imm,1);
		}
		else if(s==I_IMMW)
		{
			swrite(&args->imm,2);
		}
		else if(s==I_IMML)
		{
			swrite(&args->imm,4);
		}
		else if(s==I_IMMQ)
		{
			swrite(&args->imm,8);
		}
	}
	return 0;
}
