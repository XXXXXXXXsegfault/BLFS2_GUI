#include "../../include/build.c"
void build_https_get(char *dst)
{
	cc("app/https_get/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
