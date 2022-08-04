int jmp_handler(char *opcode,struct ins_args *args)
{
	unsigned int val;
	if(args->imm-pc-5>0x7fffffff&&args->imm-pc-5<0xffffffff80000000)
	{
		error(l->line,"branch out of range.");
	}
	val=args->imm-pc-2;
	if(val>0x7f&&val<0xffffff80)
	{
		val=val-3;
		soutc(0xe9);
		swrite(&val,4);
	}
	else
	{
		soutc(0xeb);
		swrite(&val,1);
	}
	l->needs_recompile=1;
	return 0;
}
int jcond_handler(char *opcode,struct ins_args *args)
{
	unsigned int val;
	if(args->imm-pc-6>0x7fffffff&&args->imm-pc-6<0xffffffff80000000)
	{
		error(l->line,"branch out of range.");
	}
	val=args->imm-pc-2;
	if(val>0x7f&&val<0xffffff80)
	{
		val=val-4;
		swrite(opcode+1,2);
		swrite(&val,4);
	}
	else
	{
		swrite(opcode,1);
		swrite(&val,1);
	}
	l->needs_recompile=1;
	return 0;
}
int call_handler(char *opcode,struct ins_args *args)
{
	unsigned int val;
	if(args->imm-pc-5>0x7fffffff&&args->imm-pc-5<0xffffffff80000000)
	{
		error(l->line,"branch out of range.");
	}
	val=args->imm-pc-5;
	soutc(0xe8);
	swrite(&val,4);
	l->needs_recompile=1;
	return 0;
}
void ins_init_jmp(void)
{
	ins_add("jmp I",0,0,0,0,0,0,jmp_handler);
	ins_add("jmp *%Q1",0,"\xff",1,0,0x20,I_REG1|I_MODRM,0);
	ins_add("jmp *ADDR",0,"\xff",1,0,0x20,I_MEM,0);

	ins_add("jo I",0,"\x70\x0f\x80",0,0,0,0,jcond_handler);
	ins_add("jno I",0,"\x71\x0f\x81",0,0,0,0,jcond_handler);
	ins_add("jb I",0,"\x72\x0f\x82",0,0,0,0,jcond_handler);
	ins_add("jae I",0,"\x73\x0f\x83",0,0,0,0,jcond_handler);
	ins_add("je I",0,"\x74\x0f\x84",0,0,0,0,jcond_handler);
	ins_add("jne I",0,"\x75\x0f\x85",0,0,0,0,jcond_handler);
	ins_add("jbe I",0,"\x76\x0f\x86",0,0,0,0,jcond_handler);
	ins_add("ja I",0,"\x77\x0f\x87",0,0,0,0,jcond_handler);
	ins_add("js I",0,"\x78\x0f\x88",0,0,0,0,jcond_handler);
	ins_add("jns I",0,"\x79\x0f\x89",0,0,0,0,jcond_handler);
	ins_add("jp I",0,"\x7a\x0f\x8a",0,0,0,0,jcond_handler);
	ins_add("jnp I",0,"\x7b\x0f\x8b",0,0,0,0,jcond_handler);
	ins_add("jl I",0,"\x7c\x0f\x8c",0,0,0,0,jcond_handler);
	ins_add("jge I",0,"\x7d\x0f\x8d",0,0,0,0,jcond_handler);
	ins_add("jle I",0,"\x7e\x0f\x8e",0,0,0,0,jcond_handler);
	ins_add("jg I",0,"\x7f\x0f\x8f",0,0,0,0,jcond_handler);

	ins_add("call I",0,0,0,0,0,0,call_handler);
	ins_add("call *%Q1",0,"\xff",1,0,0x10,I_REG1|I_MODRM,0);
	ins_add("call *ADDR",0,"\xff",1,0,0x10,I_MEM,0);

	ins_add("ret",0,"\xc3",1,0,0,0,0);
}
