#include "../../include/build.c"
void build_mkfs_fat(char *dst)
{
	cc("app/mkfs.fat/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
