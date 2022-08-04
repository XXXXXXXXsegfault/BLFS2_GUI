#include "../../include/build.c"
void build_init(char *dst)
{
	cc("app/init/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
