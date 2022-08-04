struct label
{
	char *name;
	unsigned long int value;
	struct label *next;
} *label_tab[1021];
void label_tab_add(char *name)
{
	int hash;
	struct label *label;
	hash=name_hash(name);
	label=xmalloc(sizeof(*label));
	label->name=name;
	label->value=pc;
	label->next=label_tab[hash];
	label_tab[hash]=label;
}
struct label *label_tab_find(char *name)
{
	int hash;
	struct label *label;
	hash=name_hash(name);
	label=label_tab[hash];
	while(label)
	{
		if(!strcmp(label->name,name))
		{
			return label;
		}
		label=label->next;
	}
	return 0;
}
