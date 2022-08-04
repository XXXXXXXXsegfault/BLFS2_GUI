#include "../include/lib.c"
int fdi,fdo;

void error(int line,int col,char *msg)
{
	char *str;
	str=xstrdup("line ");
	str=str_i_app(str,line);
	str=str_s_app(str," column ");
	str=str_i_app(str,col);
	str=str_s_app(str,": error: ");
	str=str_s_app(str,msg);
	str=str_c_app(str,'\n');
	write(2,str,strlen(str));
	exit(2);
}
int name_hash(char *str)
{
	unsigned int hash;
	hash=0;
	while(*str)
	{
		hash=(hash<<11|hash>>21)+*str;
		++str;
	}
	return hash%1021;
}
#include "load_file.c"
#include "parse/parse.c"
#include "translate/translate.c"

int main(int argc,char **argv)
{
	struct syntax_tree *root;
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
	l_global_init();
	expr_global_init();
	type_global_init();
	parse_global_init();
	load_file();
	p_current_word=l_words_head;
	root=parse_file();
	translate_file(root);
	out_flush();

	return 0;
}
