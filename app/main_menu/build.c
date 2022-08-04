#include "../../include/build.c"
void build_main_menu(char *dst)
{
	cc("app/main_menu/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
