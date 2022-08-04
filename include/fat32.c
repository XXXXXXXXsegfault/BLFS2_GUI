#ifndef _FAT32_C_
#define _FAT32_C_
struct fat32_bpb
{
	unsigned char jump[3]; //eb XX 90
	unsigned char oem_id[8]; //"MSWIN4.1"
	unsigned short int sector_size;
	unsigned char sectors_per_cluster;
	unsigned short int reserved_sectors;
	unsigned char fats;//2
	unsigned short int dirents;
	unsigned short int sectors;
	unsigned char media_type;// f0
	unsigned short int fat_size; //0
	unsigned short int sectors_per_track; //0
	unsigned short int heads;
	unsigned int part_off;
	unsigned int sectors_large;
	unsigned int fat_size_large;
	unsigned short int flags;
	unsigned short int version; //0
	unsigned int root_cluster;
	unsigned short int fsinfo;//1
	unsigned short int backup_bootsec; //6
	unsigned char rsv[12];
	unsigned char drvnum; //0x80
	unsigned char rsv2;
	unsigned char boot_sign; //0x29
	unsigned int id;
	unsigned char volume_name[11];
	unsigned char fstype[8];//"FAT32   "
	unsigned char bootcode[420];
	unsigned short int bootflag;//0xaa55
};
struct fat32_fsinfo
{
	unsigned int signature;//0x41615252
	unsigned char unused[480];
	unsigned int signature2; //0x61417272
	unsigned int free_clusters;
	unsigned int start_cluster;
	unsigned char rsv[12];
	unsigned int signature3; //0xaa550000
};
#endif
