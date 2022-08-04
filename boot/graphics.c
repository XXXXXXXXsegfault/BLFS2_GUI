struct graphics_mode_info
{
	unsigned int version;
	unsigned int hres;
	unsigned int vres;
	unsigned int pixel_format;
	unsigned int red;
	unsigned int green;
	unsigned int blue;
	unsigned int reserved;
	unsigned int line_pixels;
};
struct graphics_output_mode
{
	unsigned int maxmode;
	unsigned int mode;
	struct graphics_mode_info *info;
	unsigned long long int info_size;
	void *addr;
	unsigned long long int size;
};
struct graphics_output_protocol
{
	void *query_mode;
	void *set_mode;
	void *blt;
	struct graphics_output_mode *mode;
};
int lowest_bit(unsigned int a)
{
	int x;
	x=0;
	while(x<32)
	{
		if(a&1<<x)
		{
			return x;
		}
		++x;
	}
	return 0;
}
int highest_bit(unsigned int a)
{
	int x;
	x=32;
	while(x)
	{
		--x;
		if(a&1<<x)
		{
			return x+1;
		}
	}
	return 0;
}
unsigned int *fbstart;
unsigned int fbwidth,fbheight,fblength;
unsigned char pixformat[8];
int graphics_init(void)
{
	unsigned int guid[4];
	struct graphics_output_protocol *gop;
	struct graphics_mode_info *ginfo,*bestmode_info;
	unsigned int nmodes,x,bestmode;
	unsigned long int info_size;
	mkguid(guid,0x9042a9de,0x4a3823dc,0xde7afb96,0x6a5180d0);
	efipush(guid);
	efipush(0);
	efipush(&gop);
	eficall(efitab->boot_services->locate_prot);
	if(efi_error)
	{
		return -1;
	}
	efipush(gop);
	efipush(0);
	eficall(gop->set_mode);
	if(efi_error||gop->mode->maxmode==0)
	{
		return -1;
	}
	nmodes=gop->mode->maxmode;
	x=0;
	bestmode=0xffffffff;
	bestmode_info=0;
	info_size=0;
	while(x<nmodes)
	{
		efipush(gop);
		efipush(x);
		efipush(&info_size);
		efipush(&ginfo);
		eficall(gop->query_mode);
		if(!efi_error)
		{
			if(ginfo->hres<=1024&&ginfo->vres<=768&&
			(!bestmode_info||ginfo->hres>=bestmode_info->hres&&ginfo->vres>=bestmode_info->vres))
			{
				bestmode_info=ginfo;
				bestmode=x;
			}
		}
		++x;
	}
	if(bestmode==0xffffffff)
	{
		return -1;
	}
	efipush(gop);
	efipush(bestmode);
	eficall(gop->set_mode);
	if(efi_error)
	{
		return -1;
	}
	fbstart=gop->mode->addr;
	fbwidth=gop->mode->info->hres;
	fbheight=gop->mode->info->vres;
	fblength=gop->mode->info->line_pixels;
	if(gop->mode->info->pixel_format==0)
	{
		pixformat[1]=0;
		pixformat[0]=8;
		pixformat[3]=8;
		pixformat[2]=8;
		pixformat[5]=16;
		pixformat[4]=8;
		pixformat[6]=8;
		pixformat[7]=24;
	}
	else if(gop->mode->info->pixel_format==1)
	{
		pixformat[1]=16;
		pixformat[0]=8;
		pixformat[3]=8;
		pixformat[2]=8;
		pixformat[5]=0;
		pixformat[4]=8;
		pixformat[6]=8;
		pixformat[7]=24;
	}
	else
	{
		pixformat[1]=lowest_bit(gop->mode->info->red);
		pixformat[0]=highest_bit(gop->mode->info->red)-pixformat[1];
		pixformat[3]=lowest_bit(gop->mode->info->green);
		pixformat[2]=highest_bit(gop->mode->info->green)-pixformat[3];
		pixformat[5]=lowest_bit(gop->mode->info->blue);
		pixformat[4]=highest_bit(gop->mode->info->blue)-pixformat[5];
		pixformat[6]=32-pixformat[0]-pixformat[2]-pixformat[4];
		pixformat[7]=32-pixformat[6];
	}

	return 0;
}
