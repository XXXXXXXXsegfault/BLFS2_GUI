char *read_str(char **str,char c)
{
	char *s;
	char c1;
	s=0;
	s=str_c_app(s,c);
	++*str;
	while(c1=**str)
	{
		s=str_c_app(s,c1);
		if(c1==c)
		{
			break;
		}
		if(c1=='\\')
		{
			++*str;
			c1=**str;
			if(c1==0)
			{
				break;
			}
			s=str_c_app(s,c1);
		}
		++*str;
	}
	if(c1)
	{
		++*str;
	}
	return s;
}
char *skip_spaces(char *str)
{
	while(*str==32||*str=='\r'||*str=='\t'||*str=='\v')
	{
		++str;
	}
	return str;
}
int is_id(char c)
{
	if(c>='0'&&c<='9'||c>='A'&&c<='Z'||c>='a'&&c<='z'||c=='_')
	{
		return 1;
	}
	return 0;
}
int is_id2(char c)
{
	if(c>='0'&&c<='9'||c>='A'&&c<='Z'||c>='a'&&c<='z'||c=='_'||c=='$')
	{
		return 1;
	}
	return 0;
}
char *read_word(char **str)
{
	char *str1,*ret;
	str1=skip_spaces(*str);
	if(*str1==0)
	{
		return 0;
	}
	ret=0;
	if(is_id(*str1))
	{
		while(is_id2(*str1))
		{
			ret=str_c_app(ret,*str1);
			++str1;
		}
		*str=str1;
		return ret;
	}
	else if(*str1=='\'')
	{
		*str=str1;
		return read_str(str,'\'');
	}
	else if(*str1=='\"')
	{
		*str=str1;
		return read_str(str,'\"');
	}
	ret=str_c_app(ret,*str1);
	*str=str1+1;
	return ret;
}
unsigned long int const_to_num(char *str)
{
	unsigned long int ret;
	int x;
	ret=0;
	if(str[0]=='\'')
	{
		++str;
		if(str[0]=='\\')
		{
			if(str[1]=='\\')
			{
				ret='\\';
			}
			else if(str[1]=='n')
			{
				ret='\n';
			}
			else if(str[1]=='t')
			{
				ret='\t';
			}
			else if(str[1]=='v')
			{
				ret='\v';
			}
			else if(str[1]=='r')
			{
				ret='\r';
			}
			else if(str[1]=='\'')
			{
				ret='\'';
			}
			else if(str[1]=='\"')
			{
				ret='\"';
			}
			else if(str[1]=='\?')
			{
				ret='\?';
			}
			else if(str[1]>='0'&&str[1]<='7')
			{
				x=1;
				while(str[x]>='0'&&str[x]<='7')
				{
					ret=(ret<<3)+(str[x]-'0');
					++x;
				}
			}
			else if(str[1]=='x')
			{
				x=2;
				while(1)
				{
					if(str[x]>='0'&&str[x]<='9')
					{
						ret=ret*16+(str[x]-'0');
					}
					else if(str[x]>='A'&&str[x]<='F')
					{
						ret=ret*16+(str[x]-'A'+10);
					}
					else if(str[x]>='a'&&str[x]<='f')
					{
						ret=ret*16+(str[x]-'a'+10);
					}
					else
					{
						break;
					}
					++x;
				}
			}
			else
			{
				ret='\\';
			}
		}
		else
		{
			ret=str[0];
		}
	}
	else if(str[0]>='1'&&str[0]<='9')
	{
		x=0;
		while(str[x]>='0'&&str[x]<='9')
		{
			ret=ret*10+(str[x]-'0');
			++x;
		}
	}
	else if(str[1]=='X'||str[1]=='x')
	{
		x=2;
		while(1)
		{
			if(str[x]>='0'&&str[x]<='9')
			{
				ret=ret*16+(str[x]-'0');
			}
			else if(str[x]>='A'&&str[x]<='F')
			{
				ret=ret*16+(str[x]-'A'+10);
			}
			else if(str[x]>='a'&&str[x]<='f')
			{
				ret=ret*16+(str[x]-'a'+10);
			}
			else
			{
				break;
			}
			++x;
		}
	}
	else
	{
		x=0;
		while(str[x]>='0'&&str[x]<='7')
		{
			ret=(ret<<3)+(str[x]-'0');
			++x;
		}
	}
	return ret;
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
