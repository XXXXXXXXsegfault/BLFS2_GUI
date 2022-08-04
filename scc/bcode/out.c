char outc_buf[65536];
int outc_x;
void outc(char c)
{
	int n;
	if(outc_x==65536)
	{
		write(fdo,outc_buf,outc_x);
		outc_x=0;
	}
	outc_buf[outc_x]=c;
	++outc_x;
}
void out_flush(void)
{
	if(outc_x)
	{
		write(fdo,outc_buf,outc_x);
	}
}
void outs(char *str)
{
	while(*str)
	{
		outc(*str);
		++str;
	}
}
void out_label(char *label)
{
	outs("@");
	outs(label);
	outs("\n");
}
void out_label_name(char *label)
{
	outs(label);
}
void out_reg64(int reg)
{
	if(reg==1)
	{
		outs("%rbx");
	}
	else if(reg==2)
	{
		outs("%rsi");
	}
	else if(reg==3)
	{
		outs("%rdi");
	}
	else if(reg==4)
	{
		outs("%r8");
	}
	else if(reg==5)
	{
		outs("%r9");
	}
	else if(reg==6)
	{
		outs("%r12");
	}
	else if(reg==7)
	{
		outs("%r13");
	}
	else if(reg==8)
	{
		outs("%r14");
	}
	else if(reg==9)
	{
		outs("%r15");
	}
	else if(reg==10)
	{
		outs("%r10");
	}
	else if(reg==11)
	{
		outs("%r11");
	}
}
void out_reg32(int reg)
{
	if(reg==1)
	{
		outs("%ebx");
	}
	else if(reg==2)
	{
		outs("%esi");
	}
	else if(reg==3)
	{
		outs("%edi");
	}
	else if(reg==4)
	{
		outs("%r8d");
	}
	else if(reg==5)
	{
		outs("%r9d");
	}
	else if(reg==6)
	{
		outs("%r12d");
	}
	else if(reg==7)
	{
		outs("%r13d");
	}
	else if(reg==8)
	{
		outs("%r14d");
	}
	else if(reg==9)
	{
		outs("%r15d");
	}
	else if(reg==10)
	{
		outs("%r10d");
	}
	else if(reg==11)
	{
		outs("%r11d");
	}
}
void out_reg16(int reg)
{
	if(reg==1)
	{
		outs("%bx");
	}
	else if(reg==2)
	{
		outs("%si");
	}
	else if(reg==3)
	{
		outs("%di");
	}
	else if(reg==4)
	{
		outs("%r8w");
	}
	else if(reg==5)
	{
		outs("%r9w");
	}
	else if(reg==6)
	{
		outs("%r12w");
	}
	else if(reg==7)
	{
		outs("%r13w");
	}
	else if(reg==8)
	{
		outs("%r14w");
	}
	else if(reg==9)
	{
		outs("%r15w");
	}
	else if(reg==10)
	{
		outs("%r10w");
	}
	else if(reg==11)
	{
		outs("%r11w");
	}
}
void out_reg8(int reg)
{
	if(reg==1)
	{
		outs("%bl");
	}
	else if(reg==2)
	{
		outs("%sil");
	}
	else if(reg==3)
	{
		outs("%dil");
	}
	else if(reg==4)
	{
		outs("%r8b");
	}
	else if(reg==5)
	{
		outs("%r9b");
	}
	else if(reg==6)
	{
		outs("%r12b");
	}
	else if(reg==7)
	{
		outs("%r13b");
	}
	else if(reg==8)
	{
		outs("%r14b");
	}
	else if(reg==9)
	{
		outs("%r15b");
	}
	else if(reg==10)
	{
		outs("%r10b");
	}
	else if(reg==11)
	{
		outs("%r11b");
	}
}
void out_reg(int class,int reg)
{
	if(class==1||class==2)
	{
		out_reg8(reg);
	}
	else if(class==3||class==4)
	{
		out_reg16(reg);
	}
	else if(class==5||class==6)
	{
		out_reg32(reg);
	}
	else if(class==7||class==8)
	{
		out_reg64(reg);
	}
}
char *get_len(int class)
{
	if(class==1||class==2)
	{
		return "b";
	}
	else if(class==3||class==4)
	{
		return "w";
	}
	else if(class==5||class==6)
	{
		return "l";
	}
	else if(class==7||class==8)
	{
		return "q";
	}
}
void out_num64(unsigned long int n)
{
	char *str;
	str=str_i_app(0,n);
	outs(str);
	free(str);
}
void out_num32(unsigned long int n)
{
	out_num64(n&0xffffffff);
}
void out_num16(unsigned long int n)
{
	out_num64(n&0xffff);
}
void out_num8(unsigned long int n)
{
	out_num64(n&0xff);
}
void out_num(int class,unsigned long int n)
{
	if(class==1||class==2)
	{
		out_num8(n);
	}
	else if(class==3||class==4)
	{
		out_num16(n);
	}
	else if(class==5||class==6)
	{
		out_num32(n);
	}
	else if(class==7||class==8)
	{
		out_num64(n);
	}
}
void out_rax(int class)
{
	if(class==1||class==2)
	{
		outs("%al");
	}
	else if(class==3||class==4)
	{
		outs("%ax");
	}
	else if(class==5||class==6)
	{
		outs("%eax");
	}
	else if(class==7||class==8)
	{
		outs("%rax");
	}
}
void out_rcx(int class)
{
	if(class==1||class==2)
	{
		outs("%cl");
	}
	else if(class==3||class==4)
	{
		outs("%cx");
	}
	else if(class==5||class==6)
	{
		outs("%ecx");
	}
	else if(class==7||class==8)
	{
		outs("%rcx");
	}
}
void out_rdx(int class)
{
	if(class==1||class==2)
	{
		outs("%dl");
	}
	else if(class==3||class==4)
	{
		outs("%dx");
	}
	else if(class==5||class==6)
	{
		outs("%edx");
	}
	else if(class==7||class==8)
	{
		outs("%rdx");
	}
}
void out_acd(int class,int reg)
{
	if(reg==0)
	{
		out_rax(class);
	}
	else if(reg==1)
	{
		out_rcx(class);
	}
	else if(reg==2)
	{
		out_rdx(class);
	}
}
void acd_extend(int reg,int newclass,int oldclass)
{
	int size1,size2;
	size1=newclass-1>>1;
	size2=oldclass-1>>1;
	if(size1<=size2)
	{
		return;
	}
	outs("mov");
	if(size1==3&&size2==2&&!(newclass&1))
	{
		outs(" ");
		out_acd(5,reg);
		outs(",");
		out_acd(5,reg);
		outs("\n");
		return;
	}
	if(newclass&1)
	{
		outs("s");
	}
	else
	{
		outs("z");
	}
	if(size2==0)
	{
		outs("b");
	}
	else if(size2==1)
	{
		outs("w");
	}
	else if(size2==2)
	{
		outs("l");
	}
	else if(size2==3)
	{
		outs("q");
	}
	if(size1==0)
	{
		outs("b ");
	}
	else if(size1==1)
	{
		outs("w ");
	}
	else if(size1==2)
	{
		outs("l ");
	}
	else if(size1==3)
	{
		outs("q ");
	}
	out_acd(oldclass,reg);
	outs(",");
	out_acd(newclass,reg);
	outs("\n");
}
char *sgetc(char *str,char *ret)
{
	int x;
	if(str[0]=='\\')
	{
		if(str[1]=='\\')
		{
			*ret='\\';
			return str+2;
		}
		else if(str[1]=='n')
		{
			*ret='\n';
			return str+2;
		}
		else if(str[1]=='t')
		{
			*ret='\t';
			return str+2;
		}
		else if(str[1]=='v')
		{
			*ret='\v';
			return str+2;
		}
		else if(str[1]=='r')
		{
			*ret='\r';
			return str+2;
		}
		else if(str[1]=='\'')
		{
			*ret='\'';
			return str+2;
		}
		else if(str[1]=='\"')
		{
			*ret='\"';
			return str+2;
		}
		else if(str[1]=='\?')
		{
			*ret='\?';
			return str+2;
		}
		else if(str[1]>='0'&&str[1]<='7')
		{
			x=1;
			*ret=0;
			while(str[x]>='0'&&str[x]<='7')
			{
				*ret=(*ret<<3)+(str[x]-'0');
				++x;
			}
			return str+x;
		}
		else if(str[1]=='x')
		{
			x=2;
			*ret=0;
			while(1)
			{
				if(str[x]>='0'&&str[x]<='9')
				{
					*ret=*ret*16+(str[x]-'0');
				}
				else if(str[x]>='A'&&str[x]<='F')
				{
					*ret=*ret*16+(str[x]-'A'+10);
				}
				else if(str[x]>='a'&&str[x]<='f')
				{
					*ret=*ret*16+(str[x]-'a'+10);
				}
				else
				{
					break;
				}
				++x;
			}
			return str+x;
		}
		else
		{
			*ret='\\';
			return str+1;
		}
	}
	else
	{
		*ret=str[0];
		return str+1;
	}
}
void out_str(char *str)
{
	char c[2];
	c[1]=0;
	str=str+1;
	while(*str&&*str!='\"')
	{
		str=sgetc(str,c);
		outs(c);
	}
}
