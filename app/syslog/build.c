#include "../../include/build.c"
void build_syslog(char *dst)
{
	cc("app/syslog/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
