#include "../../include/build.c"
void build_html_parse(char *dst)
{
	cc("app/html_parse/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
