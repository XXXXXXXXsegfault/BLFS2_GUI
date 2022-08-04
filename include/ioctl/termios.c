#ifndef _IOCTL__TERMIOS_C_
#define _IOCTL__TERMIOS_C_
struct termios
{
	unsigned int iflag;
	unsigned int oflag;
	unsigned int cflag;
	unsigned int lflag;
	unsigned char line;
	unsigned char cc[32];
	unsigned int ispeed;
	unsigned int ospeed;
};
struct winsize
{
	unsigned short row;
	unsigned short col;
	unsigned int unused;
};
#define TCGETS 0x5401
#define TCSETS 0x5402
#define TIOCSCTTY 0x540e
#define TIOCGWINSZ 0x5413
#endif
