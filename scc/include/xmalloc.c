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
	val=128;
	while(val<size)
	{
		val=val*3>>1;
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
