void memcpy_sse(void *dst,void *src,unsigned int blocks); //1 block == 128 bytes
asm "@memcpy_sse"
asm "mov 8(%rsp),%rax"
asm "mov 16(%rsp),%rdx"
asm "mov 24(%rsp),%ecx"
asm "test %ecx,%ecx"
asm "je @memcpy_sse_end"
asm "@memcpy_sse_X"
asm "movups (%rdx),%xmm0"
asm "movups 16(%rdx),%xmm1"
asm "movups 32(%rdx),%xmm2"
asm "movups 48(%rdx),%xmm3"
asm "movups 64(%rdx),%xmm4"
asm "movups 80(%rdx),%xmm5"
asm "movups 96(%rdx),%xmm6"
asm "movups 112(%rdx),%xmm7"
asm "movups %xmm0,(%rax)"
asm "movups %xmm1,16(%rax)"
asm "movups %xmm2,32(%rax)"
asm "movups %xmm3,48(%rax)"
asm "movups %xmm4,64(%rax)"
asm "movups %xmm5,80(%rax)"
asm "movups %xmm6,96(%rax)"
asm "movups %xmm7,112(%rax)"
asm "add $128,%rax"
asm "add $128,%rdx"
asm "dec %ecx"
asm "jne @memcpy_sse_X"
asm "@memcpy_sse_end"
asm "ret"
int test_sse(void)
{
	int pid,status;
	static char buf_test[256];
	pid=fork();
	if(pid<0)
	{
		return -1;
	}
	if(pid==0)
	{
		memcpy_sse(buf_test,buf_test+128,1); //if SSE not supported, memcpy_sse will generate SIGILL
		exit(0);
	}
	status=0xffffffff;
	waitpid(pid,&status,0);
	if(status)
	{
		return -1;
	}
	return 0;
}
void memcpy_avx(void *dst,void *src,unsigned int blocks); //1 block == 128 bytes
asm "@memcpy_avx"
asm "mov 8(%rsp),%rax"
asm "mov 16(%rsp),%rdx"
asm "mov 24(%rsp),%ecx"
asm "test %ecx,%ecx"
asm "je @memcpy_sse_end"
asm "@memcpy_avx_X"
asm "vmovups (%rdx),%ymm0"
asm "vmovups 32(%rdx),%ymm1"
asm "vmovups 64(%rdx),%ymm2"
asm "vmovups 96(%rdx),%ymm3"
asm "vmovups %ymm0,(%rax)"
asm "vmovups %ymm1,32(%rax)"
asm "vmovups %ymm2,64(%rax)"
asm "vmovups %ymm3,96(%rax)"
asm "add $128,%rax"
asm "add $128,%rdx"
asm "dec %ecx"
asm "jne @memcpy_avx_X"
asm "@memcpy_avx_end"
asm "ret"
int test_avx(void)
{
	int pid,status;
	static char buf_test[256];
	pid=fork();
	if(pid<0)
	{
		return -1;
	}
	if(pid==0)
	{
		memcpy_avx(buf_test,buf_test+128,1); //if AVX not supported, memcpy_avx will generate SIGILL
		exit(0);
	}
	status=0xffffffff;
	waitpid(pid,&status,0);
	if(status)
	{
		return -1;
	}
	return 0;
}
void _memcpy(void *dst,void *src,unsigned int size)
{
	unsigned int blocks;
	if(!cpu_supports_sse)
	{
		memcpy(dst,src,size);
	}
	else if(!cpu_supports_avx)
	{
		blocks=size>>7;
		size&=127;
		memcpy_sse(dst,src,blocks);
		memcpy((char *)dst+(blocks<<7),(char *)src+(blocks<<7),size);
	}
	else
	{
		blocks=size>>7;
		size&=127;
		memcpy_avx(dst,src,blocks);
		memcpy((char *)dst+(blocks<<7),(char *)src+(blocks<<7),size);
	}
}
