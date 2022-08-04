void *palloc(int pages)
{
	void *ptr;
	ptr=0;
	efipush(0);
	efipush(2);
	efipush(pages);
	efipush(&ptr);
	eficall(efitab->boot_services->alloc_pages);
	return ptr;
}
void prelease(void *ptr,int pages)
{
	efipush(ptr);
	efipush(pages);
	eficall(efitab->boot_services->free_pages);
}
