#include "../../include/build.c"
void build_mod_probe(char *dst)
{
	cc("app/mod_probe/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
