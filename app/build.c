#include "copy/build.c"
#include "cpio/build.c"
#include "mkfs.ext2/build.c"
#include "mkfs.fat/build.c"
#include "remove/build.c"
#include "exec/build.c"
#include "mod_probe/build.c"
#include "bitblitter/build.c"
#include "window_manager/build.c"
#include "main_menu/build.c"
#include "pwd/build.c"
#include "file_manager/build.c"
#include "edit/build.c"
#include "task_manager/build.c"
#include "su/build.c"
#include "cbuild/build.c"
#include "syslog/build.c"
#include "dhcp_client/build.c"
#include "gethostbyname/build.c"
#include "http_get/build.c"
#include "https_get/build.c"
#include "html_parse/build.c"
#include "text_browser/build.c"

#include "init/build.c"
void build_apps(void)
{
	build_copy("build/root/bin/copy");
	build_cpio("build/root/bin/cpio");
	build_mkfs_ext2("build/root/bin/mkfs.ext2");
	build_mkfs_fat("build/root/bin/mkfs.fat");
	build_remove("build/root/bin/remove");
	build_exec("build/root/bin/exec");
	build_mod_probe("build/root/bin/mod_probe");
	build_bitblitter("build/root/bin/bitblitter");
	build_window_manager("build/root/bin/window_manager");
	build_main_menu("build/root/bin/main_menu");
	build_pwd("build/root/bin/pwd");
	build_file_manager("build/root/bin/file_manager");
	build_edit("build/root/bin/edit");
	build_task_manager("build/root/bin/task_manager");
	build_su("build/root/bin/su");
	build_cbuild("build/root/bin/cbuild");
	build_syslog("build/root/bin/syslog");
	build_dhcp_client("build/root/bin/dhcp_client");
	build_gethostbyname("build/root/bin/gethostbyname");
	build_http_get("build/root/bin/http_get");
	build_https_get("build/root/bin/https_get");
	build_html_parse("build/root/bin/html_parse");
	build_text_browser("build/root/bin/text_browser");
	
	build_init("build/initramfs/init");
}
