int needs_newl(char *tag)
{
	if(!strncmp(tag,"<p>",3)||!strncmp(tag,"<div",4)||
!strncmp(tag,"<ul",3)||!strncmp(tag,"<ol",3)||
!strncmp(tag,"<nav",4)||!strncmp(tag,"<tr",3)||
!strncmp(tag,"<header",7)||!strncmp(tag,"<footer",7)||
!strncmp(tag,"<article",8)||!strncmp(tag,"<section",8)||
!strncmp(tag,"<table",6))
	{
		return 1;
	}
	return 0;
}
struct token *parse_tag(struct token *tok,struct token *endt)
{
	struct token *end,*t;
	char *text;
	int s,s1;
	if(!tok)
	{
		return NULL;
	}
	while(tok->next!=tok&&tok!=endt&&(tok->str[0]!='<'||tok->str[1]=='!'))
	{
		tok=tok->next;
	}
	if(tok->next==tok||tok==endt)
	{
		return tok;
	}
	if(!strncmp(tok->str,"<header",7)||!strncmp(tok->str,"<aside",6)||
!strncmp(tok->str,"<article",8)||!strncmp(tok->str,"<table",6)||
!strncmp(tok->str,"<section",8)||!strncmp(tok->str,"<footer",7)||
!strncmp(tok->str,"<p>",3)||!strncmp(tok->str,"<td",3)||
!strncmp(tok->str,"<form",5)||!strncmp(tok->str,"<pre",4)||
!strncmp(tok->str,"<em",4)||!strncmp(tok->str,"<strong",7)||
!strncmp(tok->str,"<dfn",4)||!strncmp(tok->str,"<code",5)||
!strncmp(tok->str,"<samp",5)||!strncmp(tok->str,"<kbd",4)||
!strncmp(tok->str,"<div",4)||!strncmp(tok->str,"<b>",3)||
!strncmp(tok->str,"<i>",3)||!strncmp(tok->str,"<mark",5))
	{
		if(!strncmp(tok->str,"<form",5))
		{
			write(fdo,"FBEG",5);
			write(fdo,"ACTN",5);
			out_property(tok->str,"action");
			s1=1;
		}
		s=needs_newl(tok->str);
		end=locate_tag_end(tok);
		if(!end)
		{
			return tok->next;
		}
		tok=tok->next;
		while(tok!=end&&tok!=endt&&tok->next!=tok)
		{
			if(tok->str[0]!='<'||tok->str[1]=='!')
			{
				if(tok->str[1]!='!')
				{
					write(fdo,"TEXT",5);
					out_text(tok->str);
				}
				tok=tok->next;
			}
			else
			{
				tok=parse_tag(tok,end);
			}
		}
		if(s1)
		{
			write(fdo,"FEND",5);
		}
		if(s)
		{
			write(fdo,"NEWL",5);
		}
		if(tok==endt)
		{
			return tok;
		}
		return tok->next;
	}
	if(!strncmp(tok->str,"<html",5)||!strncmp(tok->str,"<head",5)||
!strncmp(tok->str,"<body",5)||!strncmp(tok->str,"<ol",3)||
!strncmp(tok->str,"<ul",3)||!strncmp(tok->str,"<nav",4)||
!strncmp(tok->str,"<tr",3)||!strncmp(tok->str,"<main",5))
	{
		s=needs_newl(tok->str);

		end=locate_tag_end(tok);
		if(!end)
		{
			return tok->next;
		}
		tok=tok->next;
		while(tok!=end&&tok!=endt&&tok->next!=tok)
		{
			tok=parse_tag(tok,end);
			while(tok->next!=tok&&tok!=endt&&(tok->str[0]!='<'||tok->str[1]=='!'))
			{
				tok=tok->next;
			}
		}
		if(s)
		{
			write(fdo,"NEWL",5);
		}
		if(tok==endt)
		{
			return tok;
		}
		return tok->next;
	}
	if(!strncmp(tok->str,"<h1",3)
||!strncmp(tok->str,"<h2",3)||!strncmp(tok->str,"<h3",3)||
!strncmp(tok->str,"<h4",3)||!strncmp(tok->str,"<h5",3)||
!strncmp(tok->str,"<h6",3)||!strncmp(tok->str,"<span",5)||
!strncmp(tok->str,"<noscript",9)||!strncmp(tok->str,"<th",3))
	{
		end=locate_tag_end(tok);
		if(!end)
		{
			return tok->next;
		}
		if(tok->next->str[0]=='<')
		{
			return tok->next;
		}
		write(fdo,"TEXT",5);
		out_text(tok->next->str);
		return end->next;
	}
	if(!strncmp(tok->str,"<a",2))
	{
		end=locate_tag_end(tok);
		if(!end)
		{
			return tok->next;
		}
		t=tok;
		while(tok!=end&&tok!=endt)
		{
			if(tok->str[0]!='<')
			{
				text=tok->str;
				break;
			}
			tok=tok->next;
		}
		if(tok==endt)
		{
			return tok;
		}
		if(tok==end)
		{
			return tok->next;
		}
		write(fdo,"LINK",5);
		out_property(t->str,"href");
		out_text(text);
		return end->next;
	}
	if(!strncmp(tok->str,"<img",4))
	{
		write(fdo,"LINK",5);
		out_property(tok->str,"src");
		out_property(tok->str,"alt");
		return tok->next;
	}
	if(!strncmp(tok->str,"<li",3))
	{
		end=locate_tag_end(tok);
		if(!end)
		{
			return tok->next;
		}
		tok=parse_tag(tok->next,end);
		if(tok->str[0]=='<')
		{
			return tok;
		}
		write(fdo,"TEXT",5);
		out_text(tok->str);
		write(fdo,"NEWL",5);
		return end->next;
	}
	if(!strncmp(tok->str,"<br",3))
	{
		write(fdo,"NEWL\0",5);
		return tok->next;
	}
	if(!strncmp(tok->str,"<title",5))
	{
		end=locate_tag_end(tok);
		if(!end)
		{
			return tok->next;
		}
		if(tok->next->str[0]=='<')
		{
			return tok->next;
		}
		write(fdo,"TITL",5);
		out_text(tok->next->str);
		return end->next;
	}
	if(!strncmp(tok->str,"<input",5))
	{
		text=get_property(tok->str,"type");
		if(!text)
		{
			write(fdo,"INBX",5);
			out_property(tok->str,"name");
			return tok->next;
		}
		if(text[0]=='\"')
		{
			++text;
		}
		if(!strncmp(text,"submit",4))
		{
			write(fdo,"SUBM",5);
			out_property(tok->str,"value");
		}
		else if(!strncmp(text,"hidden",6))
		{
			write(fdo,"ARGM",5);
			out_property(tok->str,"name");
			out_property(tok->str,"value");
		}
		else if(!strncmp(text,"text",4)||!strncmp(text,"search",6))
		{
			write(fdo,"INBX",5);
			out_property(tok->str,"name");
		}
		return tok->next;
	}
	end=locate_tag_end(tok);

	if(end)
	{
		return end->next;
	}
	return tok->next;
}
