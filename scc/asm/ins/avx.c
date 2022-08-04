void ins_init_avx(void)
{
	ins_add("vmovups ADDR,%Y2",0,"\xc5\xfc\x10",3,0,0,RM,0);
	ins_add("vmovups %Y2,ADDR",0,"\xc5\xfc\x11",3,0,0,RM,0);
}
