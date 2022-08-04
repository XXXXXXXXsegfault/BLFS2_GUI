#ifdef __GNUC__
#ifndef __x86_64__
#error "This program can only run on x86_64 Linux"
#endif
#ifndef __linux__
#error "This program can only run on x86_64 Linux"
#endif
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#else
asm ".entry"
asm "lea 8(%rsp),%rax"
asm "push %rax"
asm "pushq 8(%rsp)"
asm "call @main"
asm "mov %rax,%rdi"
asm "mov $231,%eax"
asm "syscall"
#include "syscall.c"
#include "mem.c"
#include "malloc.c"
#endif
#include "xmalloc.c"
#include "string.c"
