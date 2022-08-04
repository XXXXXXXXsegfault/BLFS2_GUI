int parse_num(char **str,unsigned long int *result)
{
	char *old_str,*word;
	int s;
	struct label *label;
	old_str=*str;
	*str=skip_spaces(*str);
	s=0;
	if(**str=='-')
	{
		++*str;
		s=1;
	}
	if(**str=='@')
	{
		//label
		++*str;
		word=read_word(str);
		if(!strcmp(word,"_$DATA"))
		{
			free(word);
			if(s)
			{
				*result=-data_addr;
			}
			else
			{
				*result=data_addr;
			}
			return 0;
		}
		else
		{
			label=label_tab_find(word);
			if(stage)
			{
				if(label)
				{
					free(word);
					*result=label->value;
					if(s)
					{
						*result=-*result;
					}
					return 0;
				}
				error(l->line,"label undefined.");
			}
			else
			{
				l->needs_recompile=1;
			}
		}
		free(word);
		return -2;
	}
	else if(**str>='0'&&**str<='9'||**str=='\'')
	{
		*result=const_to_num(*str);
		word=read_word(str);
		free(word);
	}
	else
	{
		*str=old_str;
		return -1;
	}
	if(s)
	{
		*result=-*result;
	}
	return 0;
}
int parse_const(char **str,unsigned long int *result)
{
	unsigned long int num;
	int ret,s,s1;
	char *old_str;
	old_str=*str;
	s=0;
	s1=0;
	*result=0;
	while(1)
	{
		ret=parse_num(str,&num);
		if(ret==-1)
		{
			*str=old_str;
			return -1;
		}
		if(ret==-2)
		{
			s=1;
		}
		if(s1)
		{
			*result-=num;
		}
		else
		{
			*result+=num;
		}
		*str=skip_spaces(*str);
		if(**str=='+')
		{
			s1=0;
			++*str;
		}
		else if(**str=='-')
		{
			s1=1;
			++*str;
		}
		else
		{
			break;
		}
	}
	if(s)
	{
		*result=0;
	}
	return 0;
}
int str_match(char **pstr,char *str)
{
	int l;
	char *old_str;
	old_str=*pstr;
	l=strlen(str);
	*pstr=skip_spaces(*pstr);
	if(strncmp(*pstr,str,l))
	{
		*pstr=old_str;
		return 0;
	}
	*pstr+=l;
	return 1;
}
int parse_reg32(char **str)
{
	if(str_match(str,"eax"))
	{
		return 0;
	}
	if(str_match(str,"ecx"))
	{
		return 1;
	}
	if(str_match(str,"edx"))
	{
		return 2;
	}
	if(str_match(str,"ebx"))
	{
		return 3;
	}
	if(str_match(str,"esp"))
	{
		return 4;
	}
	if(str_match(str,"ebp"))
	{
		return 5;
	}
	if(str_match(str,"esi"))
	{
		return 6;
	}
	if(str_match(str,"edi"))
	{
		return 7;
	}
	if(str_match(str,"r8d"))
	{
		return 8;
	}
	if(str_match(str,"r9d"))
	{
		return 9;
	}
	if(str_match(str,"r10d"))
	{
		return 10;
	}
	if(str_match(str,"r11d"))
	{
		return 11;
	}
	if(str_match(str,"r12d"))
	{
		return 12;
	}
	if(str_match(str,"r13d"))
	{
		return 13;
	}
	if(str_match(str,"r14d"))
	{
		return 14;
	}
	if(str_match(str,"r15d"))
	{
		return 15;
	}
	if(str_match(str,"eiz"))
	{
		return -2;
	}
	return -1;
}
int parse_reg16(char **str)
{
	if(str_match(str,"ax"))
	{
		return 0;
	}
	if(str_match(str,"cx"))
	{
		return 1;
	}
	if(str_match(str,"dx"))
	{
		return 2;
	}
	if(str_match(str,"bx"))
	{
		return 3;
	}
	if(str_match(str,"sp"))
	{
		return 4;
	}
	if(str_match(str,"bp"))
	{
		return 5;
	}
	if(str_match(str,"si"))
	{
		return 6;
	}
	if(str_match(str,"di"))
	{
		return 7;
	}
	if(str_match(str,"r8w"))
	{
		return 8;
	}
	if(str_match(str,"r9w"))
	{
		return 9;
	}
	if(str_match(str,"r10w"))
	{
		return 10;
	}
	if(str_match(str,"r11w"))
	{
		return 11;
	}
	if(str_match(str,"r12w"))
	{
		return 12;
	}
	if(str_match(str,"r13w"))
	{
		return 13;
	}
	if(str_match(str,"r14w"))
	{
		return 14;
	}
	if(str_match(str,"r15w"))
	{
		return 15;
	}
	return -1;
}
int parse_reg8(char **str)
{
	if(str_match(str,"al"))
	{
		return 0;
	}
	if(str_match(str,"cl"))
	{
		return 1;
	}
	if(str_match(str,"dl"))
	{
		return 2;
	}
	if(str_match(str,"bl"))
	{
		return 3;
	}
	if(str_match(str,"r8b"))
	{
		return 8;
	}
	if(str_match(str,"r9b"))
	{
		return 9;
	}
	if(str_match(str,"r10b"))
	{
		return 10;
	}
	if(str_match(str,"r11b"))
	{
		return 11;
	}
	if(str_match(str,"r12b"))
	{
		return 12;
	}
	if(str_match(str,"r13b"))
	{
		return 13;
	}
	if(str_match(str,"r14b"))
	{
		return 14;
	}
	if(str_match(str,"r15b"))
	{
		return 15;
	}
	if(str_match(str,"spl"))
	{
		return 20;
	}
	if(str_match(str,"bpl"))
	{
		return 21;
	}
	if(str_match(str,"sil"))
	{
		return 22;
	}
	if(str_match(str,"dil"))
	{
		return 23;
	}
	if(str_match(str,"ah"))
	{
		return 28;
	}
	if(str_match(str,"ch"))
	{
		return 29;
	}
	if(str_match(str,"dh"))
	{
		return 30;
	}
	if(str_match(str,"bh"))
	{
		return 31;
	}
	return -1;
}
int parse_reg64(char **str)
{
	if(str_match(str,"rax"))
	{
		return 0;
	}
	if(str_match(str,"rcx"))
	{
		return 1;
	}
	if(str_match(str,"rdx"))
	{
		return 2;
	}
	if(str_match(str,"rbx"))
	{
		return 3;
	}
	if(str_match(str,"rsp"))
	{
		return 4;
	}
	if(str_match(str,"rbp"))
	{
		return 5;
	}
	if(str_match(str,"rsi"))
	{
		return 6;
	}
	if(str_match(str,"rdi"))
	{
		return 7;
	}
	if(str_match(str,"r8"))
	{
		return 8;
	}
	if(str_match(str,"r9"))
	{
		return 9;
	}
	if(str_match(str,"r10"))
	{
		return 10;
	}
	if(str_match(str,"r11"))
	{
		return 11;
	}
	if(str_match(str,"r12"))
	{
		return 12;
	}
	if(str_match(str,"r13"))
	{
		return 13;
	}
	if(str_match(str,"r14"))
	{
		return 14;
	}
	if(str_match(str,"r15"))
	{
		return 15;
	}
	if(str_match(str,"riz"))
	{
		return -2;
	}
	return -1;
}
int parse_creg(char **str)
{
	if(str_match(str,"cr0"))
	{
		return 0;
	}
	if(str_match(str,"cr2"))
	{
		return 2;
	}
	if(str_match(str,"cr3"))
	{
		return 3;
	}
	if(str_match(str,"cr4"))
	{
		return 4;
	}
	if(str_match(str,"cr8"))
	{
		return 8;
	}
	return -1;
}
int parse_xreg(char **str)
{
	if(str_match(str,"xmm0"))
	{
		return 0;
	}
	if(str_match(str,"xmm10"))
	{
		return 10;
	}
	if(str_match(str,"xmm11"))
	{
		return 11;
	}
	if(str_match(str,"xmm12"))
	{
		return 12;
	}
	if(str_match(str,"xmm13"))
	{
		return 13;
	}
	if(str_match(str,"xmm14"))
	{
		return 14;
	}
	if(str_match(str,"xmm15"))
	{
		return 15;
	}
	if(str_match(str,"xmm1"))
	{
		return 1;
	}
	if(str_match(str,"xmm2"))
	{
		return 2;
	}
	if(str_match(str,"xmm3"))
	{
		return 3;
	}
	if(str_match(str,"xmm4"))
	{
		return 4;
	}
	if(str_match(str,"xmm5"))
	{
		return 5;
	}
	if(str_match(str,"xmm6"))
	{
		return 6;
	}
	if(str_match(str,"xmm7"))
	{
		return 7;
	}
	if(str_match(str,"xmm8"))
	{
		return 8;
	}
	if(str_match(str,"xmm9"))
	{
		return 9;
	}
	return -1;
}
int parse_yreg(char **str)
{
	if(str_match(str,"ymm0"))
	{
		return 0;
	}
	if(str_match(str,"ymm10"))
	{
		return 10;
	}
	if(str_match(str,"ymm11"))
	{
		return 11;
	}
	if(str_match(str,"ymm12"))
	{
		return 12;
	}
	if(str_match(str,"ymm13"))
	{
		return 13;
	}
	if(str_match(str,"ymm14"))
	{
		return 14;
	}
	if(str_match(str,"ymm15"))
	{
		return 15;
	}
	if(str_match(str,"ymm1"))
	{
		return 1;
	}
	if(str_match(str,"ymm2"))
	{
		return 2;
	}
	if(str_match(str,"ymm3"))
	{
		return 3;
	}
	if(str_match(str,"ymm4"))
	{
		return 4;
	}
	if(str_match(str,"ymm5"))
	{
		return 5;
	}
	if(str_match(str,"ymm6"))
	{
		return 6;
	}
	if(str_match(str,"ymm7"))
	{
		return 7;
	}
	if(str_match(str,"ymm8"))
	{
		return 8;
	}
	if(str_match(str,"ymm9"))
	{
		return 9;
	}
	return -1;
}
struct addr
{
	char bit32;
	char reg1;
	char reg2;
	char scale;
	unsigned int offset;
};
int parse_addr(char **str,struct addr *addr)
{
	char *old_str;
	unsigned long int off;
	old_str=*str;
	addr->offset=0;
	if(!str_match(str,"("))
	{
		if(parse_const(str,&off)==-1)
		{
			return -1;
		}
		if(off>0x7fffffff&&off<0xffffffff80000000)
		{
			error(l->line,"address out of range.");
		}
		addr->offset=off;
		if(!str_match(str,"("))
		{
			addr->bit32=0;
			addr->reg1=-1;
			addr->reg2=-1;
			addr->scale=0;
			return 0;
		}
	}
	if(!str_match(str,"%"))
	{
		error(l->line,"register required.");
	}
	if((addr->reg1=parse_reg64(str))!=-1)
	{
		addr->bit32=0;
		if(str_match(str,")"))
		{
			addr->reg2=-1;
			addr->scale=0;
			return 0;
		}
		else if(!str_match(str,","))
		{
			error(l->line,"expected \')\' or \',\' after register.");
		}
		if(!str_match(str,"%"))
		{
			error(l->line,"register required.");
		}
		if((addr->reg2=parse_reg64(str))==-1)
		{
			error(l->line,"invalid index register.");
		}
		if(addr->reg2==4)
		{
			error(l->line,"invalid index register.");
		}
		if(str_match(str,")"))
		{
			addr->scale=0;
			return 0;
		}
		else if(!str_match(str,","))
		{
			error(l->line,"expected \')\' or \',\' after register.");
		}
		if(str_match(str,"1"))
		{
			addr->scale=0;
		}
		else if(str_match(str,"2"))
		{
			addr->scale=0x40;
		}
		else if(str_match(str,"4"))
		{
			addr->scale=0x80;
		}
		else if(str_match(str,"8"))
		{
			addr->scale=0xc0;
		}
		else
		{
			error(l->line,"invalid scaler.");
		}
		if(!str_match(str,")"))
		{
			error(l->line,"expected \')\' after scaler.");
		}
		return 0;
	}
	else if((addr->reg1=parse_reg32(str))!=-1)
	{
		addr->bit32=1;
		if(str_match(str,")"))
		{
			addr->reg2=-1;
			addr->scale=0;
			return 0;
		}
		else if(!str_match(str,","))
		{
			error(l->line,"expected \')\' or \',\' after register.");
		}
		if(!str_match(str,"%"))
		{
			error(l->line,"register required.");
		}
		if((addr->reg2=parse_reg32(str))==-1)
		{
			error(l->line,"invalid index register.");
		}
		if(addr->reg2==4)
		{
			error(l->line,"invalid index register.");
		}
		if(str_match(str,")"))
		{
			addr->scale=0;
			return 0;
		}
		else if(!str_match(str,","))
		{
			error(l->line,"expected \')\' or \',\' after register.");
		}
		if(str_match(str,"1"))
		{
			addr->scale=0;
		}
		else if(str_match(str,"2"))
		{
			addr->scale=0x40;
		}
		else if(str_match(str,"4"))
		{
			addr->scale=0x80;
		}
		else if(str_match(str,"8"))
		{
			addr->scale=0xc0;
		}
		else
		{
			error(l->line,"invalid scaler.");
		}
		if(!str_match(str,")"))
		{
			error(l->line,"expected \')\' after scaler.");
		}
		return 0;
	}
	else if(str_match(str,"rip"))
	{
		if(!str_match(str,")"))
		{
			error(l->line,"expected \')\' after %rip.");
		}
		addr->bit32=0;
		addr->reg1=16;
		addr->reg2=-1;
		addr->scale=0;
		return 0;
	}
	else
	{
		error(l->line,"invalid address.");
	}
}
struct ins_args
{
	struct addr addr;
	char reg1;
	char reg2;
	char pad[6];
	unsigned long int imm;
};
int get_ins_args(char *input,char *format,struct ins_args *args)
{
	char *word;
	while(word=read_word(&format))
	{
		if(!strcmp(word,"ADDR"))
		{
			if(parse_addr(&input,&args->addr))
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"B1"))
		{
			if((args->reg1=parse_reg8(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"B2"))
		{
			if((args->reg2=parse_reg8(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"W1"))
		{
			if((args->reg1=parse_reg16(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"W2"))
		{
			if((args->reg2=parse_reg16(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"L1"))
		{
			if((args->reg1=parse_reg32(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"L2"))
		{
			if((args->reg2=parse_reg32(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"Q1"))
		{
			if((args->reg1=parse_reg64(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"Q2"))
		{
			if((args->reg2=parse_reg64(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"C1"))
		{
			if((args->reg1=parse_creg(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"C2"))
		{
			if((args->reg2=parse_creg(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"X1"))
		{
			if((args->reg1=parse_xreg(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"X2"))
		{
			if((args->reg2=parse_xreg(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"Y1"))
		{
			if((args->reg1=parse_yreg(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"Y2"))
		{
			if((args->reg2=parse_yreg(&input))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!strcmp(word,"I"))
		{
			if((parse_const(&input,&args->imm))==-1)
			{
				free(word);
				return -1;
			}
		}
		else if(!str_match(&input,word))
		{
			free(word);
			return -1;
		}
		free(word);
	}
	input=skip_spaces(input);
	if(*input!=0)
	{
		return -1;
	}
	return 0;
}
