#include "../../include/build.c"
void build_task_manager(char *dst)
{
	cc("app/task_manager/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
