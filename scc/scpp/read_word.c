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
char *read_word(char **str)
{
	char *str1,*ret;
	str1=skip_spaces(*str);
	if(str1!=*str)
	{
		*str=str1;
		return xstrdup(" ");
	}
	if(*str1==0)
	{
		return 0;
	}
	ret=0;
	if(is_id(*str1))
	{
		while(is_id(*str1))
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
	++*str;
	return ret;
}
