#include "format.c"
struct ins
{
	char *name;
	char *format;
	char *prefix;
	char *opcode;
	char opcode_len;
	char rex;
	short int modrm;
	unsigned int flags;
	int (*special_handler)(char *,struct ins_args *);
	struct ins *next;
} *ins_list[1021];
#define I_MODRM 1
#define I_IMM 2
#define I_IMMB 0
#define I_IMMW 4
#define I_IMML 8
#define I_IMMQ 12
#define I_IMMMASK 12
#define I_MEM 16
#define I_REG1 32
#define I_REG2 64
#define I_U 128

#define I8R (I_IMM|I_IMMB|I_REG1)
#define I16R (I_IMM|I_IMMW|I_REG1)
#define I32R (I_IMM|I_IMML|I_REG1)
#define I64R (I_IMM|I_IMMQ|I_REG1)
#define RR (I_REG1|I_REG2)
#define I8M (I_IMM|I_IMMB|I_MEM)
#define I16M (I_IMM|I_IMMW|I_MEM)
#define I32M (I_IMM|I_IMML|I_MEM)
#define I64M (I_IMM|I_IMMQ|I_MEM)
#define RM (I_REG2|I_MEM)

void ins_add(char *format,char *prefix,char *opcode,int opcode_len,int rex,int modrm,unsigned int flags,int (*special_handler)(char *,struct ins_args *))
{
	struct ins *ins;
	int hash;
	hash=format_hash(format);
	ins=xmalloc(sizeof(*ins));
	ins->name=read_word(&format);
	ins->format=format;
	ins->prefix=prefix;
	ins->opcode=opcode;
	ins->opcode_len=opcode_len;
	ins->rex=rex;
	ins->modrm=modrm;
	ins->flags=flags;
	ins->special_handler=special_handler;
	hash+=name_hash(ins->name);
	hash=hash%1021;
	ins->next=ins_list[hash];
	ins_list[hash]=ins;
}
#include "default.c"
#include "mov.c"
#include "jmp.c"
#include "basic_op.c"
#include "xchg.c"
#include "io.c"
#include "system.c"
#include "sse.c"
#include "avx.c"
void ins_write(char *input)
{
	struct ins *ins;
	struct ins_args args;
	char *p,*word;
	int hash;
	hash=format_hash(input);
	word=read_word(&input);
	hash+=name_hash(word);
	hash=hash%1021;
	ins=ins_list[hash];
	while(ins)
	{
		if(!strcmp(ins->name,word))
		{
			if(!get_ins_args(input,ins->format,&args))
			{
				if(ins->special_handler)
				{
					if(!ins->special_handler(ins->opcode,&args))
					{
						free(word);
						return;
					}
				}
				else
				{
					if(!write_default_ins(ins,&args))
					{
						free(word);
						return;
					}
				}
			}
		}
		ins=ins->next;
	}
	error(l->line,"unknown instruction.");
}
void ins_init(void)
{
	ins_add("nop",0,"\x90",1,0,0,0,0);
	ins_init_mov();
	ins_init_jmp();
	ins_init_basic_op();
	ins_init_xchg();
	ins_init_io();
	ins_init_system();
	ins_init_sse();
	ins_init_avx();
}
