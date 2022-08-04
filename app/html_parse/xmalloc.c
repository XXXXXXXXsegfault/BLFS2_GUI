void *xmalloc(long int size)
{
	void *ptr;
	ptr=malloc(size);
	if(ptr==0)
	{
		write(2,"FATAL: cannot allocate memory.\n",31);
		exit(1);
	}
	return ptr;
}
long int __str_size(long int size)
{
	long int val;
	int x;
	x=0;
	val=128;
	while(val<size&&x<128)
	{
		val=val+128*x;
		++x;
	}
	while(val<size)
	{
		val=val*5>>2;
	}
	return val;
}
char *xstrdup(char *str)
{
	long int l;
	char *new_str;
	l=strlen(str);
	new_str=xmalloc(__str_size(l+1));
	memcpy(new_str,str,l);
	new_str[l]=0;
	return new_str;
}
char *str_c_app(char *s,int c,int size)
{
	char *new_str;
	long int l,l1,l2;
	if(size<5)
	{
		size=0;
	}
	else
	{
		size-=5;
	}
	if(s==0)
	{
		new_str=xmalloc(128);
		new_str[0]=c;
		new_str[1]=0;
	}
	else
	{
		l=strlen(s+size)+size;
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
