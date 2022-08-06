#define _BUILD_INTERNAL_
#include "boot/build.c"
#include "app/build.c"
#include "install/build.c"
#include "include/malloc.c"
#include "include/ioctl/termios.c"

void copy_file(char *src,char *dst)
{
	char *argv[4];
	argv[0]="copy";
	argv[1]=src;
	argv[2]=dst;
	argv[3]=0;
	xexec_program("build/root/bin/copy",argv);
}
void mkrootid(void)
{
	int fd;
	char rootid[16];
	fd=open("build/tmp/rootid",578,0644);
	if(fd<0)
	{
		exit(1);
	}
	getrandom(rootid,16,1);
	write(fd,rootid,16);
	close(fd);
}
void init_fd(void)
{
	struct termios term;
	int fd;
	fd=open("build.log",578,0644);
	if(fd<0)
	{
		exit(1);
	}
	if(ioctl(1,TCGETS,&term))
	{
		dup2(fd,1);
	}
	if(ioctl(2,TCGETS,&term))
	{
		dup2(fd,2);
	}
	close(fd);
}
int main(void)
{
	char *msg;
	char *argv[5];
	init_fd();
	msg="Start Building\n";
	write(1,msg,strlen(msg));
	xmkdir("build",0755);
	xmkdir("build/tmp",0755);
	xmkdir("build/boot",0755);
	xmkdir("build/root",0755);
	xmkdir("build/root/bin",0755);
	xmkdir("build/root/lib",0755);
	xmkdir("build/root/opt",0755);
	xmkdir("build/mnt",0755);
	xmkdir("build/initramfs",0755);
	build_boot("build/boot/bootx64.efi");
	build_apps();
	mkrootid();
	copy_file("build/tmp/rootid","build/initramfs/rootid");
	copy_file("bin/scpp","build/root/bin/scpp");
	copy_file("bin/scc","build/root/bin/scc");
	copy_file("bin/bcode","build/root/bin/bcode");
	copy_file("bin/asm","build/root/bin/asm");
	argv[0]="cpio";
	argv[1]="-p";
	argv[2]="build/initramfs";
	argv[3]="build/tmp/initramfs";
	argv[4]=0;
	xexec_program("build/root/bin/cpio",argv);
	xmkdir("build/root/src",0755);
	copy_file("app","build/root/src/app");
	copy_file("boot","build/root/src/boot");
	copy_file("build.c","build/root/src/build.c");
	copy_file("include","build/root/src/include");
	copy_file("include","build/root/include");
	copy_file("install","build/root/src/install");
	copy_file("syscfg","build/root");
	copy_file("syscfg","build/root/src/syscfg");
	copy_file("src/openssl/apps/openssl","build/root/opt/openssl");
	xmkdir("build/root/src/scc",0755);
	xmkdir("build/root/src/src",0755);
	xmkdir("build/root/src/src/openssl",0755);
	xmkdir("build/root/src/src/openssl/apps",0755);
	copy_file("src/openssl/apps/openssl","build/root/src/src/openssl/apps/openssl");
	copy_file("scc/asm","build/root/src/scc/asm");
	copy_file("scc/bcode","build/root/src/scc/bcode");
	copy_file("scc/include","build/root/src/scc/include");
	copy_file("scc/scc","build/root/src/scc/scc");
	copy_file("scc/scpp","build/root/src/scc/scpp");
	build_installer("build/install.run");
	write(1,"OK\n",3);

	return 0;
}
