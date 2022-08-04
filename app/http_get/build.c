#include "../../include/build.c"
void build_http_get(char *dst)
{
	cc("app/http_get/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
