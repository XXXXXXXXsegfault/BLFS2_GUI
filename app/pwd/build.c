#include "../../include/build.c"
void build_pwd(char *dst)
{
	cc("app/pwd/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
