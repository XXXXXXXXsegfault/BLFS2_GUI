#define DRM_CLIENT_CAP_UNIVERSAL_PLANES 2

#define DRM_IOCTL_MODE_GETRESOURCES 0xc04064a0
#define DRM_IOCTL_SET_CLIENT_CAP 0x4010640d
#define DRM_IOCTL_MODE_GETPLANERESOURCES 0xc01064b5
#define DRM_IOCTL_MODE_GETCONNECTOR 0xc05064a7
#define DRM_IOCTL_MODE_CREATE_DUMB 0xc02064b2
#define DRM_IOCTL_MODE_MAP_DUMB 0xc01064b3
#define DRM_IOCTL_MODE_DESTROY_DUMB 0xc00464b4
#define DRM_IOCTL_MODE_ADDFB 0xc01c64ae
#define DRM_IOCTL_MODE_ADDFB2 0xc06864b8
#define DRM_IOCTL_MODE_RMFB 0xc00464af
#define DRM_IOCTL_MODE_SETCRTC 0xc06864a2
#define DRM_IOCTL_MODE_SETPLANE 0xc03064b7
#define DRM_IOCTL_MODE_PAGE_FLIP 0xc01864b0
#define DRM_IOCTL_MODE_GETPLANE 0xc02064b6
#define DRM_IOCTL_WAIT_VBLANK 0xc018643a
struct drm_mode_card_res
{
	unsigned int *fb_id_ptr;
	unsigned int *crtc_id_ptr;
	unsigned int *connector_id_ptr;
	unsigned int *encoder_id_ptr;
	unsigned int count_fbs;
	unsigned int count_crtcs;
	unsigned int count_connectors;
	unsigned int count_encoders;
	unsigned int min_width;
	unsigned int max_width;
	unsigned int min_height;
	unsigned int max_height;
};
struct drm_set_client_cap
{
	unsigned long long int capability;
	unsigned long long int value;
};
struct drm_mode_get_plane_res
{
	unsigned int *plane_id_ptr;
	unsigned int count_planes;
	unsigned int pad;
};
struct drm_mode_modeinfo
{
	unsigned int clock;
	unsigned short int hdisplay;
	unsigned short int hsync_start;
	unsigned short int hsync_end;
	unsigned short int htotal;
	unsigned short int hskew;
	unsigned short int vdisplay;
	unsigned short int vsync_start;
	unsigned short int vsync_end;
	unsigned short int vtotal;
	unsigned short int vscan;
	unsigned int vrefresh;
	unsigned int flags;
	unsigned int type;
	char name[32];
};
struct drm_mode_get_connector
{
	unsigned int *encoders_ptr;
	struct drm_mode_modeinfo *modes_ptr;
	unsigned int *props_ptr;
	unsigned long long int *prop_values_ptr;
	unsigned int count_modes;
	unsigned int count_props;
	unsigned int count_encoders;
	unsigned int encoder_id;
	unsigned int connector_id;
	unsigned int connector_type;
	unsigned int connector_type_id;
	unsigned int connection;
	unsigned int mm_width;
	unsigned int mm_height;
	unsigned int subpixel;
	unsigned int pad;
};
struct drm_mode_map_dumb
{
	unsigned int handle;
	unsigned int pad;
	unsigned long long int offset;
};
struct drm_mode_destroy_dumb
{
	unsigned int handle;
};
struct drm_mode_create_dumb
{
	unsigned int height;
	unsigned int width;
	unsigned int bpp;
	unsigned int flags;
	unsigned int handle;
	unsigned int pitch;
	unsigned long long int size;
};
struct drm_mode_set_plane
{
	unsigned int plane_id;
	unsigned int crtc_id;
	unsigned int fb_id;
	unsigned int flags;
	int crtc_x;
	int crtc_y;
	unsigned int crtc_w;
	unsigned int crtc_h;
	unsigned int src_x;
	unsigned int src_y;
	unsigned int src_h;
	unsigned int src_w;
};
struct drm_mode_fb_cmd
{
	unsigned int fb_id;
	unsigned int width;
	unsigned int height;
	unsigned int pitch;
	unsigned int bpp;
	unsigned int depth;
	unsigned int handle;
};
struct drm_mode_crtc
{
	unsigned int *set_connectors_ptr;
	unsigned int count_connectors;
	unsigned int crtc_id;
	unsigned int fb_id;
	unsigned int x;
	unsigned int y;
	unsigned int gamma_size;
	unsigned int mode_valid;
	struct drm_mode_modeinfo mode;
};
struct drm_mode_crtc_page_flip
{
	unsigned int crtc_id;
	unsigned int fb_id;
	unsigned int flags;
	unsigned int reserved;
	void *user_data;
};
struct drm_mode_get_plane
{
	unsigned int plane_id;
	unsigned int crtc_id;
	unsigned int fb_id;
	unsigned int possible_crtcs;
	unsigned int gamma_size;
	unsigned int count_format_types;
	unsigned int *format_type_ptr;
};
struct drm_mode_fb_cmd2
{
	unsigned int fb_id;
	unsigned int width;
	unsigned int height;
	unsigned int pixel_format;
	unsigned int flags;
	unsigned int handles[4];
	unsigned int pitches[4];
	unsigned int offsets[4];
	unsigned long long int modifier[4];
};
struct drm_wait_vblank_request
{
	unsigned int type;
	unsigned int sequence;
	unsigned long int signal;
};
struct drm_wait_vblank_reply
{
	unsigned int type;
	unsigned int sequence;
	long int tval_sec;
	long int tval_usec;
};

void drm_mode_release_res_info(struct drm_mode_card_res *info)
{
	free(info->fb_id_ptr);
	free(info->crtc_id_ptr);
	free(info->connector_id_ptr);
	free(info->encoder_id_ptr);
	free(info);
}
struct drm_mode_card_res *drm_mode_get_res_info(int fd)
{
	struct drm_mode_card_res *ptr;
	if((ptr=malloc(sizeof(*ptr)))==0)
	{
		return (void *)0;
	}
	memset(ptr,0,sizeof(*ptr));
	if(ioctl(fd,DRM_IOCTL_MODE_GETRESOURCES,ptr))
	{
		drm_mode_release_res_info(ptr);
		return (void *)0;
	}
	if(ptr->count_fbs)
	{
		if((ptr->fb_id_ptr=malloc(ptr->count_fbs*4))==0)
		{
			drm_mode_release_res_info(ptr);
			return (void *)0;
		}
	}
	if(ptr->count_crtcs)
	{
		if((ptr->crtc_id_ptr=malloc(ptr->count_crtcs*4))==0)
		{
			drm_mode_release_res_info(ptr);
			return (void *)0;
		}
	}
	if(ptr->count_connectors)
	{
		if((ptr->connector_id_ptr=malloc(ptr->count_connectors*4))==0)
		{
			drm_mode_release_res_info(ptr);
			return (void *)0;
		}
	}
	if(ptr->count_encoders)
	{
		if((ptr->encoder_id_ptr=malloc(ptr->count_encoders*4))==0)
		{
			drm_mode_release_res_info(ptr);
			return (void *)0;
		}
	}
	if(ioctl(fd,DRM_IOCTL_MODE_GETRESOURCES,ptr))
	{
		drm_mode_release_res_info(ptr);
		return (void *)0;
	}
	return ptr;
}
void drm_mode_release_plane_res_info(struct drm_mode_get_plane_res *info)
{
	free(info->plane_id_ptr);
	free(info);
}
struct drm_mode_get_plane_res *drm_mode_get_plane_res_info(int fd)
{
	struct drm_mode_get_plane_res *ptr;
	if((ptr=malloc(sizeof(*ptr)))==0)
	{
		return (void *)0;
	}
	memset(ptr,0,sizeof(*ptr));
	if(ioctl(fd,DRM_IOCTL_MODE_GETPLANERESOURCES,ptr))
	{
		drm_mode_release_plane_res_info(ptr);
		return (void *)0;
	}
	if(ptr->count_planes)
	{
		if((ptr->plane_id_ptr=malloc(ptr->count_planes*4))==0)
		{
			drm_mode_release_plane_res_info(ptr);
			return (void *)0;
		}
	}
	if(ioctl(fd,DRM_IOCTL_MODE_GETPLANERESOURCES,ptr))
	{
		drm_mode_release_plane_res_info(ptr);
		return (void *)0;
	}
	return ptr;
}
void drm_mode_release_connector_info(struct drm_mode_get_connector *info)
{
	free(info->encoders_ptr);
	free(info->modes_ptr);
	free(info->props_ptr);
	free(info->prop_values_ptr);
	free(info);
}
struct drm_mode_get_connector *drm_mode_get_connector_info(int fd,unsigned int connector_id)
{
	struct drm_mode_get_connector *ptr;
	if((ptr=malloc(sizeof(*ptr)))==0)
	{
		return (void *)0;
	}
	memset(ptr,0,sizeof(*ptr));
	ptr->connector_id=connector_id;
	if(ioctl(fd,DRM_IOCTL_MODE_GETCONNECTOR,ptr))
	{
		drm_mode_release_connector_info(ptr);
		return (void *)0;
	}
	if(ptr->count_encoders)
	{
		if((ptr->encoders_ptr=malloc(ptr->count_encoders*4))==0)
		{
			drm_mode_release_connector_info(ptr);
			return (void *)0;
		}
	}
	if(ptr->count_modes)
	{
		if((ptr->modes_ptr=malloc(ptr->count_modes*sizeof(struct drm_mode_modeinfo)))==0)
		{
			drm_mode_release_connector_info(ptr);
			return (void *)0;
		}
	}
	if(ptr->count_props)
	{
		if((ptr->props_ptr=malloc(ptr->count_props*4))==0)
		{
			drm_mode_release_connector_info(ptr);
			return (void *)0;
		}
		if((ptr->prop_values_ptr=malloc(ptr->count_props*8))==0)
		{
			drm_mode_release_connector_info(ptr);
			return (void *)0;
		}
	}
	if(ioctl(fd,DRM_IOCTL_MODE_GETCONNECTOR,ptr))
	{
		drm_mode_release_connector_info(ptr);
		return (void *)0;
	}
	return ptr;
}
void drm_wait_vblank(int fd)
{
	struct drm_wait_vblank_reply vb;
	memset(&vb,0,sizeof(vb));
	vb.type=1;
	vb.sequence=1;
	ioctl(fd,DRM_IOCTL_WAIT_VBLANK,&vb);

}
