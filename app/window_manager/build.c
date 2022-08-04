#include "../../include/build.c"
void build_window_manager(char *dst)
{
	cc("app/window_manager/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
