void **block_handles;
long long int count_block_handles,bufsize;
struct EFI_block_io_media
{
	unsigned int id;
	char removable;
	char present;
	char logicalpartition;
	char readonly;
	char writecaching;
	char pad[3];
	unsigned int blocksize;
	unsigned int align;
	unsigned int pad2;
	long long int last_block;
	//unused
};
struct EFI_block_io
{
	unsigned long long int rev;
	struct EFI_block_io_media *media;
	void *reset;
	void *readblocks;
	void *writeblocks;
	void *flushblocks;
} *bio;
int read_blocks(struct EFI_block_io *bio,long long int start,long long int size,void *buf)
{
	if(start>bio->media->last_block)
	{
		return 0;
	}
	if(start*bio->media->blocksize+size>(bio->media->last_block+1)*bio->media->blocksize)
	{
		size=(bio->media->last_block+1-start)*bio->media->blocksize;
	}
	efipush(bio);
	efipush(bio->media->id);
	efipush(start);
	efipush(size);
	efipush(buf);
	eficall(bio->readblocks);
	if(efi_error)
	{
		return 0;
	}
	return size;
}
int block_init(void)
{
	unsigned int bio_guid[4];
	int x;
	struct EFI_block_io *c_bio;
	static unsigned char boot_signature[4096];
	mkguid(bio_guid,0x964e5b21,0x11d26459,0xa000398e,0x3b7269c9);
	efipush(2);
	efipush(bio_guid);
	efipush(0);
	efipush(&count_block_handles);
	efipush(block_handles);
	eficall(efitab->boot_services->locate_handle);
	if(efi_error!=5||count_block_handles==0)
	{
		return -1;
	}
	bufsize=count_block_handles;
	block_handles=palloc(bufsize+4095>>12);
	if(block_handles==0)
	{
		return -1;
	}
	efipush(2);
	efipush(bio_guid);
	efipush(0);
	efipush(&bufsize);
	efipush(block_handles);
	eficall(efitab->boot_services->locate_handle);
	count_block_handles>>=3;
	x=0;
	while(x<count_block_handles)
	{
		c_bio=0;
		efipush(block_handles[x]);
		efipush(bio_guid);
		efipush(&c_bio);
		eficall(efitab->boot_services->handle_prot);
		if(!(efi_error||c_bio==0||c_bio->media->blocksize==0))
		{
			if(c_bio->media->logicalpartition&&read_blocks(c_bio,0,4096,boot_signature)==4096&&!memcmp(boot_signature,bootid,16))
			{
				bio=c_bio;
				return 0;
			}
		}
		++x;
	}
	return -1;
}
