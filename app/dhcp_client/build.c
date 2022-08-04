#include "../../include/build.c"
void build_dhcp_client(char *dst)
{
	cc("app/dhcp_client/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
