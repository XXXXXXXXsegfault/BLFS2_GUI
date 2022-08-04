void ins_init_sse(void)
{
	ins_add("movups %X2,%X1",0,"\x0f\x10",2,0,0,RR|I_MODRM,0);
	ins_add("movups ADDR,%X2",0,"\x0f\x10",2,0,0,RM,0);
	ins_add("movups %X2,ADDR",0,"\x0f\x11",2,0,0,RM,0);
}
