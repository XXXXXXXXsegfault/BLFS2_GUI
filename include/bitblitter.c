#ifndef _BITBLITTER_C_
#define _BITBLITTER_C_
struct bbreq
{
	unsigned int cmd;
	short int x;
	short int y;
	short int w;
	short int h;
};
#define BBCMD_BLIT_XRGB 0x40
#define BBCMD_BLIT_MRGB 0x41
#define BBCMD_GET_SIZE 0x50
#define BBCMD_CLEAR 0x60
#define BBCMD_DISPLAY 0x61
#define BBCMD_SET_PZ 0x62
#endif
