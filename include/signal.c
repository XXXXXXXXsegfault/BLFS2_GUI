#ifndef _SIGNAL_C_
#define _SIGNAL_C_
#include "syscall.c"
#include "mem.c"
struct sigaction
{
	void (*handler)(int);
	int flags;
	int pad;
	void (*restorer)(void);
	unsigned long long int mask[16];
};
struct sigset
{
	unsigned long long int val[16];
};
#define SIGHUP 1
#define SIGINT 2
#define SIGQUIT 3
#define SIGILL 4
#define SIGTRAP 5
#define SIGABRT 6
#define SIGBUS 7
#define SIGFPE 8
#define SIGKILL 9
#define SIGUSR1 10
#define SIGSEGV 11
#define SIGUSR2 12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM 15
#define SIGSTKFLT 16
#define SIGCHLD 17
#define SIGCONT 18
#define SIGSTOP 19
#define SIGTSTP 20
#define SIGTTIN 21
#define SIGTTOU 22
#define SIGURG 23
#define SIGXCPU 24
#define SIGXFSZ 25
#define SIGVTALRM 26
#define SIGPROF 27
#define SIGWINCH 28
#define SIGIO 29
#define SIGPWR 30
#define SIGSYS 31
void __def_sigreturn(void);
asm "@__def_sigreturn"
asm "mov $15,%eax"
asm "syscall"
asm "jmp @__def_sigreturn"
#define SIG_DFL ((void *)0)
#define SIG_IGN ((void *)1)
#define SA_RESTORER 0x4000000
// NOTE: The SIG argument of signal handler is unreliable in SCC environment.
void (*signal(int sig,void (*handler)(int)))(int)
{
	struct sigaction act,old_act;
	memset(&act,0,sizeof(act));
	act.handler=handler;
	act.flags=SA_RESTORER;
	act.restorer=__def_sigreturn;
	syscall(13,sig,&act,&old_act,8,0,0);
	return old_act.handler;
}
#endif
