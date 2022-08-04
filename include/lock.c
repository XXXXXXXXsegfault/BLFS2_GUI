#ifndef _LOCK_C_
#define _LOCK_C_
#include "syscall.c"
unsigned int lock_set32(unsigned int *ptr,unsigned int value)
{
	asm "mov 16(%rbp),%rcx"
	asm "mov 24(%rbp),%eax"
	asm "xchg %eax,(%rcx)"
}
void spin_lock(unsigned int *ptr)
{
	while(lock_set32(ptr,1))
	{
		while(*ptr)
		{
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
			asm ".word 0x90f3"
		}
	}
}
void spin_unlock(unsigned int *ptr)
{
	*ptr=0;
}
unsigned int mutex_wait;
void mutex_lock(unsigned int *ptr)
{
	while(lock_set32(ptr,1))
	{
		syscall(202,ptr,0,1,0,0,0);
	}
}
void mutex_unlock(unsigned int *ptr)
{
	*ptr=0;
	syscall(202,ptr,1,1,0,0,0);
}


#endif
