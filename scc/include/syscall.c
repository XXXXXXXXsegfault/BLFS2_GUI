int open(char *name,int flags,int mode)
{
	asm "push %rdi"
	asm "push %rsi"
	asm "push %rdx"
	asm "push %r10"
	asm "push %r11"
	asm "mov 16(%rbp),%rdi"
	asm "mov 24(%rbp),%rsi"
	asm "mov 32(%rbp),%rdx"
	asm "mov $2,%eax"
	asm "syscall"
	asm "pop %r11"
	asm "pop %r10"
	asm "pop %rdx"
	asm "pop %rsi"
	asm "pop %rdi"
}
int openat(int dirfd,char *name,int flags,int mode)	
{
	asm "push %rdi"
	asm "push %rsi"
	asm "push %rdx"
	asm "push %r10"
	asm "push %r11"
	asm "mov 16(%rbp),%rdi"
	asm "mov 24(%rbp),%rsi"
	asm "mov 32(%rbp),%rdx"
	asm "mov 40(%rbp),%r10"
	asm "mov $257,%eax"
	asm "syscall"
	asm "pop %r11"
	asm "pop %r10"
	asm "pop %rdx"
	asm "pop %rsi"
	asm "pop %rdi"
}
int close(int fd)
{
	asm "push %rdi"
	asm "push %r10"
	asm "push %r11"
	asm "mov 16(%rbp),%rdi"
	asm "mov $3,%eax"
	asm "syscall"
	asm "pop %r11"
	asm "pop %r10"
	asm "pop %rdi"
}
int read(int fd,void *buf,int size)
{
	asm "push %rdi"
	asm "push %rsi"
	asm "push %rdx"
	asm "push %r10"
	asm "push %r11"
	asm "mov 16(%rbp),%rdi"
	asm "mov 24(%rbp),%rsi"
	asm "mov 32(%rbp),%rdx"
	asm "mov $0,%eax"
	asm "syscall"
	asm "pop %r11"
	asm "pop %r10"
	asm "pop %rdx"
	asm "pop %rsi"
	asm "pop %rdi"
}
int write(int fd,void *buf,int size)
{
	asm "push %rdi"
	asm "push %rsi"
	asm "push %rdx"
	asm "push %r10"
	asm "push %r11"
	asm "mov 16(%rbp),%rdi"
	asm "mov 24(%rbp),%rsi"
	asm "mov 32(%rbp),%rdx"
	asm "mov $1,%eax"
	asm "syscall"
	asm "pop %r11"
	asm "pop %r10"
	asm "pop %rdx"
	asm "pop %rsi"
	asm "pop %rdi"
}
/*
void *mmap(void *start,unsigned long int size,int prot,int flags,int fd,unsigned long int off)
{
	asm "push %rdi"
	asm "push %rsi"
	asm "push %rdx"
	asm "push %r10"
	asm "push %r11"
	asm "push %r8"
	asm "push %r9"
	asm "mov 16(%rbp),%rdi"
	asm "mov 24(%rbp),%rsi"
	asm "mov 32(%rbp),%rdx"
	asm "mov 40(%rbp),%r10"
	asm "mov 48(%rbp),%r8"
	asm "mov 56(%rbp),%r9"
	asm "mov $9,%eax"
	asm "syscall"
	asm "pop %r9"
	asm "pop %r8"
	asm "pop %r11"
	asm "pop %r10"
	asm "pop %rdx"
	asm "pop %rsi"
	asm "pop %rdi"
}
int munmap(void *start,unsigned long int size)
{
	asm "push %rdi"
	asm "push %rsi"
	asm "push %r10"
	asm "push %r11"
	asm "mov 16(%rbp),%rdi"
	asm "mov 24(%rbp),%rsi"
	asm "mov $11,%eax"
	asm "syscall"
	asm "pop %r11"
	asm "pop %r10"
	asm "pop %rsi"
	asm "pop %rdi"
}
*/
void *brk(void *ptr)
{
	asm "push %rdi"
	asm "push %r10"
	asm "push %r11"
	asm "mov 16(%rbp),%rdi"
	asm "mov $12,%eax"
	asm "syscall"
	asm "pop %r11"
	asm "pop %r10"
	asm "pop %rdi"
}
void exit(int code)
{
	asm "mov 16(%rbp),%rdi"
	asm "mov $231,%eax"
	asm "syscall"
}
