#include "../../include/build.c"
void build_cbuild(char *dst)
{
	cc("app/cbuild/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
