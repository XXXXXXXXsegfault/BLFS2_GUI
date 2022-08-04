#include "../../include/build.c"
void build_bitblitter(char *dst)
{
	cc("app/bitblitter/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
