void ins_init_system(void)
{
	ins_add("int $I",0,"\xcd",1,0,0,I_IMM|I_IMMB|I_U,0);
	ins_add("int3",0,"\xcc",1,0,0,0,0);
	ins_add("syscall",0,"\x0f\x05",2,0,0,0,0);
	ins_add("cli",0,"\xfa",1,0,0,0,0);
	ins_add("sti",0,"\xfb",1,0,0,0,0);
	ins_add("hlt",0,"\xf4",1,0,0,0,0);
	ins_add("lgdt ADDR",0,"\x0f\x01",2,0,0x10,I_MEM,0);
	ins_add("lidt ADDR",0,"\x0f\x01",2,0,0x18,I_MEM,0);
	ins_add("ltr %W1",0,"\x0f\x00",2,0,0xd8,I_REG1|I_MODRM,0);
	ins_add("mov %C2,%Q1",0,"\x0f\x20",2,0,0,RR|I_MODRM,0);
	ins_add("mov %Q1,%C2",0,"\x0f\x22",2,0,0,RR|I_MODRM,0);
	ins_add("iretq",0,"\xcf",1,0x48,0,0,0);
	ins_add("lretq",0,"\xcb",1,0x48,0,0,0);
	ins_add("rdmsr",0,"\x0f\x32",2,0,0,0,0);
	ins_add("wrmsr",0,"\x0f\x30",2,0,0,0,0);
	ins_add("cpuid",0,"\x0f\xa2",2,0,0,0,0);
	ins_add("pushfq",0,"\x9c",1,0,0,0,0);
	ins_add("popfq",0,"\x9d",1,0,0,0,0);
}
