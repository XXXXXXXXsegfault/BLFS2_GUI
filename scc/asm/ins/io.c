void ins_init_io(void)
{
	ins_add("in $I,%al",0,"\xe4",1,0,0,I_IMM|I_IMMB|I_U,0);
	ins_add("out %al,$I",0,"\xe6",1,0,0,I_IMM|I_IMMB|I_U,0);
}
