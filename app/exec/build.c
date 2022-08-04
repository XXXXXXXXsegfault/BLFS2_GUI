#include "../../include/build.c"
void build_exec(char *dst)
{
	cc("app/exec/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
