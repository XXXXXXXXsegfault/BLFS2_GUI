#include "../../include/build.c"
void build_copy(char *dst)
{
	cc("app/copy/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
