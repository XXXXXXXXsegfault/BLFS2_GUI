int movl_handler(char *opcode,struct ins_args *args)
{
	if(args->reg1>=8)
	{
		soutc(0x41);
	}
	soutc(0xb8|args->reg1&7);
	swrite(&args->imm,4);
	return 0;
}
int movq_handler(char *opcode,struct ins_args *args)
{
	if(args->reg1>=8)
	{
		soutc(0x49);
	}
	else
	{
		soutc(0x48);
	}
	soutc(0xb8|args->reg1&7);
	swrite(&args->imm,8);
	return 0;
}
int movw_handler(char *opcode,struct ins_args *args)
{
	soutc(0x66);
	if(args->reg1>=8)
	{
		soutc(0x41);
	}
	soutc(0xb8|args->reg1&7);
	swrite(&args->imm,2);
	return 0;
}
int movb_handler(char *opcode,struct ins_args *args)
{
	if(args->reg1>=20&&args->reg1<24)
	{
		soutc(0x40);
	}
	else if(args->reg1>=8&&args->reg1<16)
	{
		soutc(0x41);
	}
	soutc(0xb0|args->reg1&7);
	swrite(&args->imm,1);
	return 0;
}
int movq_handler2(char *opcode,struct ins_args *args)
{
	if(args->imm>0xffffffff)
	{
		return 1;
	}
	if(args->reg1>=8)
	{
		soutc(0x41);
	}
	soutc(0xb8|args->reg1&7);
	swrite(&args->imm,4);
	return 0;
}
void ins_init_mov(void)
{
	ins_add("mov %W1,%ds",0,"\x8e",1,0,0xd8,I_REG1|I_MODRM,0);
	ins_add("mov %W1,%es",0,"\x8e",1,0,0xc0,I_REG1|I_MODRM,0);
	ins_add("mov %W1,%fs",0,"\x8e",1,0,0xe0,I_REG1|I_MODRM,0);
	ins_add("mov %W1,%gs",0,"\x8e",1,0,0xe8,I_REG1|I_MODRM,0);
	ins_add("mov %W1,%ss",0,"\x8e",1,0,0xd0,I_REG1|I_MODRM,0);

	ins_add("mov %ds,%L1",0,"\x8c",1,0,0xd8,I_REG1|I_MODRM,0);
	ins_add("mov %es,%L1",0,"\x8c",1,0,0xc0,I_REG1|I_MODRM,0);
	ins_add("mov %fs,%L1",0,"\x8c",1,0,0xe0,I_REG1|I_MODRM,0);
	ins_add("mov %gs,%L1",0,"\x8c",1,0,0xe8,I_REG1|I_MODRM,0);
	ins_add("mov %ss,%L1",0,"\x8c",1,0,0xd0,I_REG1|I_MODRM,0);

	ins_add("movsbw %B1,%W2","\x66","\x0f\xbe",2,0,0,RR|I_MODRM,0);
	ins_add("movsbl %B1,%L2",0,"\x0f\xbe",2,0,0,RR|I_MODRM,0);
	ins_add("movsbq %B1,%Q2",0,"\x0f\xbe",2,0x48,0,RR|I_MODRM,0);
	ins_add("movswl %W1,%L2",0,"\x0f\xbf",2,0,0,RR|I_MODRM,0);
	ins_add("movswq %W1,%Q2",0,"\x0f\xbf",2,0x48,0,RR|I_MODRM,0);
	ins_add("movslq %L1,%Q2",0,"\x63",1,0x48,0,RR|I_MODRM,0);

	ins_add("movzbw %B1,%W2","\x66","\x0f\xb6",2,0,0,RR|I_MODRM,0);
	ins_add("movzbl %B1,%L2",0,"\x0f\xb6",2,0,0,RR|I_MODRM,0);
	ins_add("movzbq %B1,%Q2",0,"\x0f\xb6",2,0,0,RR|I_MODRM,0);
	ins_add("movzwl %W1,%L2",0,"\x0f\xb7",2,0,0,RR|I_MODRM,0);
	ins_add("movzwq %W1,%Q2",0,"\x0f\xb7",2,0,0,RR|I_MODRM,0);

	ins_add("movsbw ADDR,%W2","\x66","\x0f\xbe",2,0,0,RM,0);
	ins_add("movsbl ADDR,%L2",0,"\x0f\xbe",2,0,0,RM,0);
	ins_add("movsbq ADDR,%Q2",0,"\x0f\xbe",2,0x48,0,RM,0);
	ins_add("movswl ADDR,%L2",0,"\x0f\xbf",2,0,0,RM,0);
	ins_add("movswq ADDR,%Q2",0,"\x0f\xbf",2,0x48,0,RM,0);
	ins_add("movslq ADDR,%Q2",0,"\x63",1,0x48,0,RM,0);

	ins_add("movzbw ADDR,%W2","\x66","\x0f\xb6",2,0,0,RM,0);
	ins_add("movzbl ADDR,%L2",0,"\x0f\xb6",2,0,0,RM,0);
	ins_add("movzbq ADDR,%Q2",0,"\x0f\xb6",2,0x48,0,RM,0);
	ins_add("movzwl ADDR,%L2",0,"\x0f\xb7",2,0,0,RM,0);
	ins_add("movzwq ADDR,%Q2",0,"\x0f\xb7",2,0x48,0,RM,0);


	ins_add("movw $I,ADDR","\x66","\xc7",1,0,0,I16M|I_U,0);
	ins_add("mov ADDR,%W2","\x66","\x8b",1,0,0,RM,0);
	ins_add("mov %W2,ADDR","\x66","\x89",1,0,0,RM,0);

	ins_add("movb $I,ADDR",0,"\xc6",1,0,0,I8M|I_U,0);
	ins_add("mov ADDR,%B2",0,"\x8a",1,0,0,RM,0);
	ins_add("mov %B2,ADDR",0,"\x88",1,0,0,RM,0);

	ins_add("movl $I,ADDR",0,"\xc7",1,0,0,I32M|I_U,0);
	ins_add("mov ADDR,%L2",0,"\x8b",1,0,0,RM,0);
	ins_add("mov %L2,ADDR",0,"\x89",1,0,0,RM,0);

	ins_add("movq $I,ADDR",0,"\xc7",1,0x48,0,I32M,0);
	ins_add("mov ADDR,%Q2",0,"\x8b",1,0x48,0,RM,0);
	ins_add("mov %Q2,ADDR",0,"\x89",1,0x48,0,RM,0);

	ins_add("mov $I,%W1",0,0,0,0,0,0,movw_handler);
	ins_add("mov $I,%B1",0,0,0,0,0,0,movb_handler);
	ins_add("mov $I,%L1",0,0,0,0,0,0,movl_handler);
	ins_add("mov $I,%Q1",0,0,0,0,0,0,movq_handler);
	ins_add("mov $I,%Q1",0,"\xc7",1,0x48,0,I32R|I_MODRM,0);
	ins_add("mov $I,%Q1",0,0,0,0,0,0,movq_handler2);

	ins_add("mov %W2,%W1","\x66","\x89",1,0,0,RR|I_MODRM,0);
	ins_add("mov %B2,%B1",0,"\x88",1,0,0,RR|I_MODRM,0);
	ins_add("mov %L2,%L1",0,"\x89",1,0,0,RR|I_MODRM,0);
	ins_add("mov %Q2,%Q1",0,"\x89",1,0x48,0,RR|I_MODRM,0);

}
