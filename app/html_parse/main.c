/*
 * NOTE: Only needed HTML features are supported
 * */

#include "../../include/malloc.c"
#include "../../include/mem.c"
#define malloc malloc_nolock
#define free free_nolock
#include "xmalloc.c"
int fdi,fdo;

#include "load_file.c"
#include "parse.c"
int main(int argc,char **argv)
{
	if(argc<3)
	{
		return 1;
	}
	fdi=open(argv[1],0,0);
	if(fdi<0)
	{
		return 1;
	}
	fdo=open(argv[2],578,0644);
	if(fdo<0)
	{
		return 1;
	}
	load_file();
	if(token==NULL)
	{
		return 1;
	}
	parse_tag(token,NULL);
	if(lseek(fdo,0,1)<=0)
	{
		return 1;
	}
	return 0;
}
