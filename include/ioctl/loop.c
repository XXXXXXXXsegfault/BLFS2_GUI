#ifndef _IOCTL__LOOP_C_
#define _IOCTL__LOOP_C_
#define LOOP_CTL_GET_FREE 0x4c82
#define LOOP_SET_FD 0x4c00
#define LOOP_CLR_FD 0x4c01
#define LOOP_SET_BLOCK_SIZE 0x4c09

#define LOOP_SET_STATUS64 0x4c04
#define LOOP_GET_STATUS64 0x4c05

struct loop_info64
{
	unsigned long device;
	unsigned long inode;
	unsigned long rdevice;
	unsigned long offset;
	unsigned long sizelimit;
	unsigned int number;
	unsigned int encrypt_type;
	unsigned int encrypt_key_size;
	unsigned int flags;
	unsigned int file_name[64];
	unsigned int crypt_name[64];
	unsigned int encrypt_key[32];
	unsigned long init[2];
};

#endif
