asm "@entry"
asm "lea @startc-@__X(%rip),%rax"
asm "@__X"
asm "push %rdx"
asm "push %rcx"
asm "cmp $@startc,%rax"
asm "jne @relocate_code"
asm "@__Y"
asm "pushq $0x100000"
asm "pushq $0"
asm "pushq $0x100000"
asm "call @memset"
asm "add $24,%rsp"
asm "call @main"
asm "add $16,%rsp"
asm "ret"
asm "@relocate_code"
asm "mov %rax,%r8"
asm "pushq $@_memmove_end-@_memmove_start"
asm "lea @_memmove_start-@startc(%rax),%rcx"
asm "push %rcx"
asm "pushq $0x1000000"
asm "call @memmove"
asm "add $24,%rsp"
asm "pushq $@end_code-@startc"
asm "push %r8"
asm "pushq $@startc"
asm "pushq $@__Y1"
asm "mov $0x1000000+@memmove-@_memmove_start,%rcx"
asm "jmp *%rcx"
asm "@__Y1"
asm "add $24,%rsp"
asm "jmp @__Y"

#include "../include/mem.c"
#include "efi.c"
struct EFI_system_table *efitab;
void *efihandle;
void bootid(void); // filled by build.c
#include "palloc.c"
#include "graphics.c"
#include "block.c"
#include "../include/ext2.c"
#include "ext2_load.c"
#include "do_boot.c"
int main(void *handle,struct EFI_system_table *tab)
{
	efihandle=handle;
	efitab=tab;
	if(graphics_init())
	{
		return -1;
	}
	if(block_init())
	{
		return -2;
	}
	if(ext2_init())
	{
		return -3;
	}
	return boot_init();
}
