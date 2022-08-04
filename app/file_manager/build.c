#include "../../include/build.c"
void build_file_manager(char *dst)
{
	cc("app/file_manager/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
