
char *str_c_app(char *s,int c)
{
	char *new_str;
	long int l,l1,l2;
	if(s==0)
	{
		new_str=xmalloc(128);
		new_str[0]=c;
		new_str[1]=0;
	}
	else
	{
		l=strlen(s);
		l1=__str_size(l+1);
		l2=__str_size(l+2);
		if(l1==l2)
		{
			new_str=s;
			new_str[l]=c;
			new_str[l+1]=0;
		}
		else
		{
			new_str=xmalloc(l2);
			memcpy(new_str,s,l);
			new_str[l]=c;
			new_str[l+1]=0;
			free(s);
		}
	}
	return new_str;
}
char *str_s_app(char *s,char *s2)
{
	while(*s2)
	{
		s=str_c_app(s,*s2);
		s2=s2+1;
	}
	return s;
}
char *str_i_app(char *s,unsigned long int n)
{
	unsigned long int a;
	a=10000000000000000000;
	int c;
	if(n==0)
	{
		return str_c_app(s,'0');
	}
	while(a>n)
	{
		a/=10;
	}
	while(a)
	{
		c=n/a;
		n%=a;
		a/=10;
		s=str_c_app(s,c+'0');
	}
	return s;
}
