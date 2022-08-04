#ifndef _STAT_C_
#define _STAT_C_
struct stat
{
	unsigned long int dev;
	unsigned long int ino;
	unsigned long int nlink;
	unsigned int mode;
	unsigned int uid;
	unsigned int gid;
	unsigned int pad1;
	unsigned long int rdev;
	unsigned long int size;
	unsigned long int blksize;
	unsigned long int blocks;
	unsigned long int atime;
	unsigned long int atime1;
	unsigned long int mtime;
	unsigned long int mtime1;
	unsigned long int ctime;
	unsigned long int ctime1;
	unsigned long int pad2[3];
};
#define STAT_DIR 040000
#define STAT_REG 0100000
#define STAT_FIFO 010000
#define STAT_SOCK 0140000
#define STAT_LNK 0120000
#define STAT_CHR 020000
#define STAT_BLK 060000
#endif
