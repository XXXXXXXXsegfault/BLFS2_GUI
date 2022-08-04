struct EFI_input_interface
{
	void *reset;
	void *read_key;
	void *event;
};
struct EFI_output_interface
{
	void *reset;
	void *output_string;
	void *test_string;
	void *query_mode;
	void *set_mode;
	void *set_attribute;
	void *clear_screen;
	void *set_cursor_position;
	void *enable_cursor;
	void *c_mode;
};
struct EFI_runtime_services
{
	unsigned int header[6];
	void *gettime;
	void *settime;
	void *getwakeuptime;
	void *setwakeuptime;
	void *setmmap;
	void *convptr;
	void *getval;
	void *getnextval;
	void *setval;
	void *get_next_high_mono_count;
	void *reset_system;
	void *updatecap;
	void *querycapcap;
	void *queryvar;
};
struct EFI_boot_services
{
	unsigned int header[6];
	void *raise_tpl;
	void *restore_tpl;
	void *alloc_pages;
	void *free_pages;
	void *getmemmap;
	void *alloc_pool;
	void *free_pool;
	void *create_event;
	void *set_timer;
	void *waitforevent;
	void *signalevent;
	void *close_event;
	void *check_event;
	void *install_interf;
	void *reinstall_interf;
	void *uninstall_interf;
	void *handle_prot;
	void *handle_prot2;
	void *register_prot;
	void *locate_handle;
	void *locate_devpath;
	void *install_config;
	void *loadimg;
	void *startimg;
	void *exit;
	void *unload;
	void *exit_boot_services;
	void *get_next_mono_count;
	void *stall;
	void *set_watchdog_timer;
	void *connect_controller;
	void *disconnect_controller;
	void *open_prot;
	void *close_prot;
	void *open_prot_info;
	void *prots_per_handle;
	void *locate_handle_buf;
	void *locate_prot;
	void *install_multiple_prot;
	void *uninstall_multiple_prot;
	void *crc32;
	void *copy_mem;
	void *set_mem;
	void *create_event_ex;
};
struct EFI_config_table
{
	unsigned int guid[4];
	void *addr;
};
struct EFI_system_table
{
	unsigned int header[6];
	short int *fwvendor;
	unsigned int fwrev;
	unsigned int pad;
	void *coninhandle;
	struct EFI_input_interface *conin;
	void *conouthandle;
	struct EFI_output_interface *conout;
	void *conerrhandle;
	struct EFI_output_interface *conerr;
	struct EFI_runtime_services *runtime_services;
	struct EFI_boot_services *boot_services;
	unsigned long long int n_entries;
	struct EFI_config_table *config_table;
};
int efi_count_args;
int efi_error;
unsigned long int efi_args[24];
void _efipush(unsigned long arg)
{
	efi_args[efi_count_args]=arg;
	++efi_count_args;
}
unsigned long int efi_status;
#define efipush(a) _efipush((unsigned long int)(a))
unsigned long int _eficall(void *fun,int count,unsigned long int *args)
{
	asm "push %rbx"
	asm "push %rsi"
	asm "push %rdi"
	asm "push %r8"
	asm "push %r9"
	asm "push %r10"
	asm "push %r11"
	asm "push %r12"
	asm "push %r13"
	asm "push %r14"
	asm "push %r15"
	asm "mov %rsp,%r15"
	asm "mov 24(%rbp),%eax"
	asm "mov 32(%rbp),%rbx"
	asm "and $0xf0,%spl"
	asm "test %eax,%eax"
	asm "je @_eficall_X1"
	asm "@_eficall_X2"
	asm "pushq -8(%rbx,%rax,8)"
	asm "dec %eax"
	asm "jne @_eficall_X2"
	asm "@_eficall_X1"
	asm "mov 24(%rbx),%r9"
	asm "mov 16(%rbx),%r8"
	asm "mov 8(%rbx),%rdx"
	asm "mov (%rbx),%rcx"
	asm "call *16(%rbp)"
	asm "mov %r15,%rsp"
	asm "pop %r15"
	asm "pop %r14"
	asm "pop %r13"
	asm "pop %r12"
	asm "pop %r11"
	asm "pop %r10"
	asm "pop %r9"
	asm "pop %r8"
	asm "pop %rdi"
	asm "pop %rsi"
	asm "pop %rbx"
}
void eficall(void *fun)
{
	efi_status=_eficall(fun,efi_count_args,efi_args);
	efi_count_args=0;
	efi_error=0;
	if(efi_status>=0x8000000000000000)
	{
		efi_error=efi_status&0x7fffffffffffffff;
	}
}
unsigned int *mkguid(unsigned int *ptr,unsigned int a1,unsigned int a2,unsigned int a3,unsigned int a4)
{
	ptr[0]=a1;
	ptr[1]=a2;
	ptr[2]=a3;
	ptr[3]=a4;
	return ptr;
}
