#include "../../include/malloc.c"
#include "../../include/mem.c"
char *str_app(char *str,int c)
{
	long int l;
	char *new_str;
	l=0;
	if(str)
	{
		l=strlen(str);
	}
	if(l==0||(l&63)==63)
	{
		new_str=malloc((l+2>>6)+1<<6);
		if(new_str==NULL)
		{
			exit(12);
		}
		memcpy(new_str,str,l);
		new_str[l]=c;
		new_str[l+1]=0;
		return new_str;
	}
	else
	{
		str[l]=c;
		str[l+1]=0;
		return str;
	}
}
int main(int argc,char **argv)
{
	char **args,**args_new,*str;
	int n,x,c;
	if(argc<2)
	{
		return 22;
	}
	str=NULL;
	args=NULL;
	x=0;
	n=0;
	while(c=argv[1][x])
	{
		if(c==32)
		{
			if(str)
			{
				if(n==0||(n&15)==15)
				{
					args_new=malloc((n+1>>4)+1<<4);
					if(args_new==NULL)
					{
						return 12;
					}
					memcpy(args_new,args,n*sizeof(char *));
					free(args);
					args=args_new;
				}
				args[n]=str;
				++n;
			}
			str=NULL;
		}
		else
		{
			if(c=='\\')
			{
				++x;
				c=argv[1][x];
				if(c==0)
				{
					break;
				}
			}
			str=str_app(str,c);
		}
		++x;
	}
	if(str)
	{
		if(n==0||(n&15)==15)
		{
			args_new=malloc((n+1>>4)+1<<4);
			if(args_new==NULL)
			{
				return 12;
			}
			memcpy(args_new,args,n*sizeof(char *));
			free(args);
			args=args_new;
		}
		args[n]=str;
		++n;
	}
	if(n==0)
	{
		return 22;
	}
	args[n]=NULL;
	return -execv(args[0],args);
}
