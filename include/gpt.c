#ifndef _GPT_C_
#define _GPT_C_
struct mbr_entry
{
	unsigned char boot;
	unsigned char chs_start[3];
	unsigned char type;
	unsigned char chs_end[3];
	unsigned int lba_start;
	unsigned int lba_size;
};
struct gpt_header
{
	char signature[8];
	unsigned int revision; //0x10000
	unsigned int header_size; // 0x5c
	unsigned int header_crc32;
	unsigned int rsv;
	unsigned long lba;
	unsigned long lba_alter;
	unsigned long first_usable;
	unsigned long last_usable;
	unsigned char guid[16];
	unsigned long entries_start;
	unsigned int entries_count;
	unsigned int entry_size;
	unsigned int entries_crc32;
};
struct gpt_entry
{
	unsigned char type[16];
	unsigned char guid[16];
	unsigned long start;
	unsigned long end;
	unsigned long attr;
	unsigned char name[72];
};
unsigned char gpt_crc32_reverse(unsigned char val)
{
	unsigned char ret;
	ret=val&1;
	ret=ret<<1|val>>1&1;
	ret=ret<<1|val>>2&1;
	ret=ret<<1|val>>3&1;
	ret=ret<<1|val>>4&1;
	ret=ret<<1|val>>5&1;
	ret=ret<<1|val>>6&1;
	ret=ret<<1|val>>7&1;
	return ret;
}
unsigned int gpt_crc32(void *buf,unsigned int size)
{
	unsigned int regs,c,x,old_regs;
	unsigned char *val;
	regs=0xffffffff;
	val=buf;
	while(size)
	{
		c=gpt_crc32_reverse(*val);
		x=8;
		do
		{
			--x;
			old_regs=regs;
			regs<<=1;
			if(old_regs>>31^c>>x&1)
			{
				regs^=0x04c11db7;
			}
		}
		while(x);
		++val;
		--size;
	}
	old_regs=regs;
	regs=gpt_crc32_reverse(old_regs);
	regs=regs<<8|gpt_crc32_reverse(old_regs>>8);
	regs=regs<<8|gpt_crc32_reverse(old_regs>>16);
	regs=regs<<8|gpt_crc32_reverse(old_regs>>24);
	return regs^0xffffffff;
}

#endif
