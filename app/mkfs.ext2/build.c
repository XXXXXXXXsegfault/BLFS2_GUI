#include "../../include/build.c"
void build_mkfs_ext2(char *dst)
{
	cc("app/mkfs.ext2/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
