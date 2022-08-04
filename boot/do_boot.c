void *locate_acpi(void)
{
	int x;
	long long int status;
	unsigned int guid[4];
	unsigned int guid2[4];
	x=0;
	mkguid(guid,0xeb9d2d30,0x11d32d88,0x9000169a,0x4dc13f27);
	mkguid(guid2,0x8868e871,0x11d3e4f1,0x800022bc,0x81883cc7);
	while(x<efitab->n_entries)
	{
		if(!memcmp(efitab->config_table[x].guid,guid,16))
		{
			return efitab->config_table[x].addr;
		}
		if(!memcmp(efitab->config_table[x].guid,guid2,16))
		{
			return efitab->config_table[x].addr;
		}
		++x;
	}
	return 0;
}

struct linux_screen_info
{
	unsigned char orig_x;
	unsigned char orig_y;
	unsigned short int ext_mem;
	unsigned short int orig_video_page;
	unsigned char orig_video_mode;
	unsigned char orig_video_cols;
	unsigned char flags;
	unsigned char rsv1;
	unsigned short int orig_video_ega_bx;
	unsigned short int rsv2;
	unsigned char orig_video_lines;
	unsigned char orig_video_isvga;
	unsigned short int orig_video_points;
	unsigned short int width;
	unsigned short int height;
	unsigned short int depth;
	unsigned int base;
	unsigned int size;
	unsigned short int magic;
	unsigned short int offset;
	unsigned short int line_length;
	unsigned char red_size;
	unsigned char red_pos;
	unsigned char green_size;
	unsigned char green_pos;
	unsigned char blue_size;
	unsigned char blue_pos;
	unsigned char rsvd_size;
	unsigned char rsvd_pos;
	unsigned short int vesapm_seg;
	unsigned short int vesapm_off;
	unsigned short int pages;
	unsigned short int attributes;
	unsigned int cap;
	unsigned int ext_base;
	unsigned short int unused2;
};
struct e820_entry
{
	unsigned long long int addr;
	unsigned long long int size;
	unsigned int type;
};
struct linux_boot_params
{
	struct linux_screen_info sinfo;
	char unused1[0x80];
	unsigned int ext_ramdisk_image;
	unsigned int ext_ramdisk_size;
	unsigned int ext_cmdline; 
	char unused2[0x11c];
	unsigned char n_e820;
	char unused3[8];
	char setup_sects;//0x1f1
	char unused4[0x8];
	unsigned short int vid_mode;//0x1fa
	char unused5[4];
	char jump[2];
	unsigned int magic;
	unsigned short int version;//0x206
	char unused6[0x8];
	unsigned char loader_type; //enter 0xff
	unsigned char load_flags; //0x211 enter 0x80  
	char unused7[6];
	unsigned int ramdisk_image;
	unsigned int ramdisk_size; //0x21c
	char unused8[8];
	unsigned int cmdline; //0x228
	char unused9[0xa4];
	struct e820_entry e820_table[1];
};

struct EFI_md
{
	unsigned int type;
	unsigned int pad;
	unsigned long long int paddr;
	unsigned long long int vaddr;
	unsigned long long int pages;
	unsigned long long int attr;
};

int fill_e820(struct linux_boot_params *boot_params)
{
	struct EFI_md *meminfo,*info;
	unsigned long long int size;
	unsigned long long int key;
	unsigned long long int desc_size;
	unsigned int version;
	unsigned int np;
	unsigned int nentries,x;
	meminfo=0;
	size=0;
	x=0;
	efipush(&size);
	efipush(meminfo);
	efipush(&key);
	efipush(&desc_size);
	efipush(&version);
	eficall(efitab->boot_services->getmemmap);
	if(efi_error!=5)
	{
		return 1;
	}
	np=size+2*desc_size+4095>>12;
	if((meminfo=palloc(np))==0)
	{
		return 1;
	}
	efipush(&size);
	efipush(meminfo);
	efipush(&key);
	efipush(&desc_size);
	efipush(&version);
	eficall(efitab->boot_services->getmemmap);
	if(efi_error)
	{
		return 1;
	}
	nentries=size/desc_size;
	info=meminfo;
	while(x<nentries)
	{
		boot_params->e820_table[x].addr=info->paddr;
		boot_params->e820_table[x].size=info->pages<<12;
		if(info->type==1||info->type==2||info->type==3||info->type==4||info->type==7)
		{
			boot_params->e820_table[x].type=1;
		}
		else if(info->type==9)
		{
			boot_params->e820_table[x].type=3;
		}
		else if(info->type==10)
		{
			boot_params->e820_table[x].type=4;
		}
		else if(info->type==14)
		{
			boot_params->e820_table[x].type=7;
		}
		else
		{
			boot_params->e820_table[x].type=2;
		}
		info=(void *)((char *)info+desc_size);
		++x;
	}
	boot_params->n_e820=nentries;
	efipush(efihandle);
	efipush(key);
	eficall(efitab->boot_services->exit_boot_services);
	if(efi_error)
	{
		prelease(meminfo,np);
		return 1;
	}
	return 0;
}
char boot_cmdline[512];
void do_boot(void *image,void *boot_params);
int boot_init(void)
{
	struct ext2_image kernel,initramfs;
	struct linux_boot_params *boot_params;
	void *linux_buf;
	unsigned int end_header;
	int x,x1;
	unsigned long int acpi_addr;
	strcpy(boot_cmdline,"console=tty0 rw loglevel=4 init=/init acpi_rsdp=0x");
	x=strlen(boot_cmdline);
	acpi_addr=(long)locate_acpi();
	x1=64;
	do
	{
		x1-=4;
		boot_cmdline[x]="0123456789abcdef"[(acpi_addr>>x1&0xf)];
		++x;
	}
	while(x1);
	boot_cmdline[x]=0;

	if(ext2_load_path("/vmlinuz"))
	{
		return -4;
	}
	if(ext2_image_load(&kernel))
	{
		return -5;
	}
	if(ext2_load_path("/initramfs"))
	{
		return -6;
	}
	if(ext2_image_load(&initramfs))
	{
		return -7;
	}
	end_header=kernel.data[0x201];
	if((boot_params=palloc(1))==0)
	{
		return -8;
	}
	memset(boot_params,0,4096);
	boot_params->sinfo.orig_video_mode=3;
	boot_params->sinfo.orig_video_cols=0x50;
	boot_params->sinfo.orig_video_ega_bx=0x03;
	boot_params->sinfo.orig_video_lines=0x19;
	boot_params->sinfo.orig_video_isvga=0x23;
	boot_params->sinfo.orig_video_points=0x10;
	boot_params->sinfo.width=fbwidth;
	boot_params->sinfo.height=fbheight;
	boot_params->sinfo.depth=32;
	boot_params->sinfo.base=(long)fbstart;
	boot_params->sinfo.ext_base=(long)fbstart>>32;
	boot_params->sinfo.size=(unsigned int)fbheight*fblength*4;
	boot_params->sinfo.line_length=fblength*4;
	boot_params->sinfo.red_size=pixformat[0];
	boot_params->sinfo.red_pos=pixformat[1];
	boot_params->sinfo.green_size=pixformat[2];
	boot_params->sinfo.green_pos=pixformat[3];
	boot_params->sinfo.blue_size=pixformat[4];
	boot_params->sinfo.blue_pos=pixformat[5];
	boot_params->sinfo.rsvd_size=pixformat[6];
	boot_params->sinfo.rsvd_pos=pixformat[7];
	boot_params->sinfo.cap=2;
	end_header+=0x202;
	memcpy((char *)boot_params+0x1f1,kernel.data+0x1f1,end_header-0x1f1);
	if(boot_params->magic!=0x53726448||boot_params->version<0x20b)
	{
		return -9;
	}
	boot_params->ramdisk_image=(long)initramfs.data;
	boot_params->ext_ramdisk_image=(long)initramfs.data>>32;
	boot_params->ramdisk_size=initramfs.size;
	boot_params->ext_ramdisk_size=initramfs.size>>32;
	boot_params->cmdline=(long)boot_cmdline;
	boot_params->ext_cmdline=(long)boot_cmdline>>32;
	boot_params->loader_type=0xff;
	boot_params->load_flags|=0x20;
	if((linux_buf=palloc(0x30000))==0)
	{
		return -10;
	}
	if(fill_e820(boot_params))
	{
		return -11;
	}
	memcpy(linux_buf,kernel.data,kernel.size);
	do_boot(linux_buf,boot_params);
}
asm "@gdt"
asm ".quad 0,0,0x00af9a000000ffff"
asm ".quad 0x00af92000000ffff"
asm "@gdtr"
asm ".word 31"
asm ".quad @gdt"
asm "@do_boot"
asm "mov 8(%rsp),%rdi"
asm "mov 16(%rsp),%rsi"
asm "pushq $0x10"
asm "pushq $@do_boot_X1"
asm "lgdt @gdtr"
asm "mov $0x18,%ax"
asm "mov %ax,%ds"
asm "mov %ax,%es"
asm "mov %ax,%fs"
asm "mov %ax,%gs"
asm "mov %ax,%ss"
asm "cli"
asm "lretq"
asm "@do_boot_X1"
asm "movzbl 0x1f1(%rdi),%eax"
asm "test %eax,%eax"
asm "jne @do_boot_X2"
asm "mov $4,%al"
asm "@do_boot_X2"
asm "add $2,%eax"
asm "shl $9,%eax"
asm "add %rdi,%rax"
asm "jmp *%rax"
