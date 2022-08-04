struct elf_header
{
	unsigned char ident[16];
	unsigned short int type;
	unsigned short int machine;
	unsigned int version;
	unsigned long int entry;
	unsigned long int phoff;
	unsigned long int shoff;
	unsigned int flags;
	unsigned short int ehsize;
	unsigned short int phentsize;
	unsigned short int phnum;
	unsigned short int shentsize;
	unsigned short int shnum;
	unsigned short int shstrndx;
} elf_header;
struct elf_program_header
{
	unsigned int type;
	unsigned int flags;
	unsigned long int offset;
	unsigned long int vaddr;
	unsigned long int paddr;
	unsigned long int filesz;
	unsigned long int memsz;
	unsigned long int align;
};
unsigned long int spos;
void swrite(void *buf,unsigned long int size)
{
	unsigned char *new_data;
	new_data=xmalloc(l->ins_len+size);
	memcpy(new_data,l->ins_buf,l->ins_len);
	memcpy(new_data+l->ins_len,buf,size);
	free(l->ins_buf);
	l->ins_buf=new_data;
	l->ins_len+=size;
	spos+=size;
	pc+=size;
}
void soutc(char c)
{
	swrite(&c,1);
}
char outc_buf[65536];
int outc_x;
void outc(char c)
{
	int n;
	if(outc_x==65536)
	{
		write(fdo,outc_buf,outc_x);
		outc_x=0;
	}
	outc_buf[outc_x]=c;
	++outc_x;
}
void out_flush(void)
{
	if(outc_x)
	{
		write(fdo,outc_buf,outc_x);
	}
}
void c_write(char *buf,int size)
{
	while(size)
	{
		outc(*buf);
		++buf;
		--size;
	}
}
void out_addr(unsigned long int addr)
{
	int x;
	x=64;
	do
	{
		x-=4;
		write(fde,"0123456789ABCDEF"+(addr>>x&0xf),1);
	}
	while(x);
	write(fde,": ",2);
}

void mkelf(void)
{
	struct elf_program_header phead[2];
	unsigned long int addr,size;
	memcpy(&elf_header,"\x7f\x45\x4c\x46\x02\x01\x01",7);
	elf_header.type=2;
	elf_header.machine=0x3e;
	elf_header.version=1;
	elf_header.phoff=0x40;
	elf_header.ehsize=0x40;
	elf_header.phentsize=0x38;
	elf_header.phnum=2;
	elf_header.shentsize=0x40;
	write(fdo,&elf_header,0x40);
	phead[0].type=1;
	phead[0].flags=0x5;
	phead[0].offset=0;
	phead[0].vaddr=0x10000;
	phead[0].paddr=0x10000;
	phead[0].filesz=spos+0xb0;
	phead[0].memsz=spos+0xb0;
	phead[0].align=0x1000;
	phead[1].type=1;
	phead[1].flags=0x6;
	phead[1].offset=0;
	phead[1].vaddr=0x20000000;
	phead[1].paddr=0x20000000;
	phead[1].filesz=0;
	phead[1].memsz=data_size;
	phead[1].align=0x1000;
	write(fdo,phead,sizeof(phead));
	l=lines_head;
	while(l)
	{
		c_write(l->ins_buf,l->ins_len);
		if(fde>=0)
		{
			out_addr(l->ins_pos);
			write(fde,l->str,strlen(l->str));
			write(fde,"\n",1);
		}
		l=l->next;
	}
	out_flush();
}
