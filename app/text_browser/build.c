#include "../../include/build.c"
void build_text_browser(char *dst)
{
	cc("app/text_browser/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
