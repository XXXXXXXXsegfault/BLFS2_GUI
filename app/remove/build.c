#include "../../include/build.c"
void build_remove(char *dst)
{
	cc("app/remove/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
