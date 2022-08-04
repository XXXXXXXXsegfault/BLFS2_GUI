#include "../../include/build.c"
void build_su(char *dst)
{
	cc("app/su/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
