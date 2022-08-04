#include "../../include/build.c"
void build_gethostbyname(char *dst)
{
	cc("app/gethostbyname/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
