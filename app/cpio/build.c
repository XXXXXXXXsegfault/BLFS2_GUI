#include "../../include/build.c"
void build_cpio(char *dst)
{
	cc("app/cpio/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
