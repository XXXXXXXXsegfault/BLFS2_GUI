#include "../../include/build.c"
void build_edit(char *dst)
{
	cc("app/edit/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
