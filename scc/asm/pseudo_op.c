void parse_pseudo_op(char *str)
{
	char *word,*p,c;
	unsigned long int val;
	if(str_match(&str,"string"))
	{
		word=read_word(&str);
		if(*word!='\"')
		{
			error(l->line,"expected string after \'.string\'.");
		}
		p=word+1;
		while(*p!='\"')
		{
			p=sgetc(p,&c);
			soutc(c);
		}
		free(word);
		soutc(0);
	}
	else if(str_match(&str,"entry"))
	{
		l->needs_recompile=1;
		elf_header.entry=pc;
	}
	else if(str_match(&str,"datasize"))
	{
		word=read_word(&str);
		if(word)
		{
			data_size=const_to_num(word);
			free(word);
		}
	}
	else if(str_match(&str,"byte"))
	{
		do
		{
			if(parse_const(&str,&val)==-1)
			{
				error(l->line,"invalid constant expression.");
			}
			swrite(&val,1);
		}
		while(str_match(&str,","));
	}
	else if(str_match(&str,"word"))
	{
		do
		{
			if(parse_const(&str,&val)==-1)
			{
				error(l->line,"invalid constant expression.");
			}
			swrite(&val,2);
		}
		while(str_match(&str,","));
	}
	else if(str_match(&str,"long"))
	{
		do
		{
			if(parse_const(&str,&val)==-1)
			{
				error(l->line,"invalid constant expression.");
			}
			swrite(&val,4);
		}
		while(str_match(&str,","));
	}
	else if(str_match(&str,"quad"))
	{
		do
		{
			if(parse_const(&str,&val)==-1)
			{
				error(l->line,"invalid constant expression.");
			}
			swrite(&val,8);
		}
		while(str_match(&str,","));
	}
	else if(str_match(&str,"offset"))
	{
		word=read_word(&str);
		if(word)
		{
			val=const_to_num(word);
			free(word);
			if(val<spos)
			{
				error(l->line,"invalid offset.");
			}
			while(val!=spos)
			{
				soutc(0);
			}
			l->needs_recompile=1;
		}
	}
	else if(str_match(&str,"addr"))
	{
		word=read_word(&str);
		if(word)
		{
			val=const_to_num(word);
			free(word);
			pc=val;
			l->needs_recompile=1;
		}
	}
	else if(str_match(&str,"dataaddr"))
	{
		word=read_word(&str);
		if(word)
		{
			data_addr=const_to_num(word);
		}
	}
	else if(str_match(&str,"align"))
	{
		word=read_word(&str);
		if(word)
		{
			val=const_to_num(word);
			if(val<20)
			{
				while((1<<val)-1&spos)
				{
					soutc(0);
				}
				l->needs_recompile=1;
			}
		}
	}
	else
	{
		error(l->line,"unknown pseudo-op.");
	}
}
