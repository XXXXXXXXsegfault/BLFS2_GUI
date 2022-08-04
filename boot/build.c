#include "../include/build.c"
void build_boot(char *dst)
{
	int fdi,fdo,n;
	static char buf[4096];
	unsigned long int bootid[2];
	cc("boot/main.c","build/tmp/tmp.asm");
	xappend("boot/peheader.asm","build/tmp/boot.asm");
	xappend("build/tmp/tmp.asm","build/tmp/boot.asm");
	// fill bootid
	xsappend("@bootid\n","build/tmp/boot.asm");
	xsappend(".quad ","build/tmp/boot.asm");
	getrandom(bootid,16,1);
	xbappend(bootid,16,"build/tmp/bootid");
	xiappend(bootid[0],"build/tmp/boot.asm");
	xsappend(",","build/tmp/boot.asm");
	xiappend(bootid[1],"build/tmp/boot.asm");
	xsappend("\n","build/tmp/boot.asm");

	xappend("boot/pe_end.asm","build/tmp/boot.asm");
	assemble("build/tmp/boot.asm","build/tmp/boot.elf");
	fdi=open("build/tmp/boot.elf",0,0);
	if(fdi<0)
	{
		exit(1);
	}
	fdo=open(dst,578,0644);
	if(fdo<0)
	{
		exit(1);
	}
	lseek(fdi,0xb0,0);
	while((n=read(fdi,buf,4096))>0)
	{
		write(fdo,buf,n);
	}
	close(fdi);
	close(fdo);
}
