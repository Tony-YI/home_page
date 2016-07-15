#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**************************Boot Entry Structer**************************/
#pragma pack(push, 1)
struct BootEntry //90 bytes
				//in 64bit system, long is 64bit, int is 32bit
				//in 32bit system, long is 32bit, int is 32bit
{
	unsigned char BS_jmpBoot[3]; /* Assembly instruction to jump to boot code */
	unsigned char BS_OEMName[8]; /* OEM Name in ASCII */
	/**/unsigned short BPB_BytsPerSec; /* Bytes per sector. Allowed values include 512,
										1024, 2048, and 4096 */
	/**/unsigned char BPB_SecPerClus; /* Sectors per cluster (data unit). Allowed values are
										powers of 2, but the cluster size must be 32KB or
										smaller */
	/**/unsigned short BPB_RsvdSecCnt; /* Size in sectors of the reserved area */
	/**/unsigned char BPB_NumFATs; /* Number of FATs */
	unsigned short BPB_RootEntCnt; /* Maximum number of files in the root directory for
										FAT12 and FAT16. This is 0 for FAT32 */
	unsigned short BPB_TotSec16; /* 16-bit value of number of sectors in file system */
	unsigned char BPB_Media; /* Media type */
	/**/unsigned short BPB_FATSz16; /* 16-bit size in sectors of each FAT for FAT12 and FAT16.
										For FAT32, this field is 0 */
	unsigned short BPB_SecPerTrk; /* Sectors per track of storage device */
	unsigned short BPB_NumHeads; /* Number of heads in storage device */
	unsigned int/*long*/ BPB_HiddSec; /* Number of sectors before the start of partition */
	/**/unsigned int/*long*/ BPB_TotSec32; /* 32-bit value of number of sectors in file system.
										Either this value or the 16-bit value above must be
										0 */
	/**/unsigned int/*long*/ BPB_FATSz32; /* 32-bit size in sectors of one FAT */
	unsigned short BPB_ExtFlags; /* A flag for FAT */
	unsigned short BPB_FSVer; /* The major and minor version number */
	/**/unsigned int/*long*/ BPB_RootClus; /* Cluster where the root directory can be found */
	unsigned short BPB_FSInfo; /* Sector where FSINFO structure can be found */
	unsigned short BPB_BkBootSec; /* Sector where backup copy of boot sector is located */
	unsigned char BPB_Reserved[12]; /* Reserved */
	unsigned char BS_DrvNum; /* BIOS INT13h drive number */
	unsigned char BS_Reserved1; /* Not used */
	unsigned char BS_BootSig; /* Extended boot signature to identify if the next three
									values are valid */
	unsigned int/*long*/ BS_VolID; /* Volume serial number */
	unsigned char BS_VolLab[11]; /* Volume label in ASCII. User defines when creating the
										file system */
	unsigned char BS_FilSysType[8]; /* File system type label in ASCII */
};
#pragma pack(pop)
/***********************************************************************/


/**************************Directery Entry Structer*********************/
#pragma pack(push,1)
struct DirEntry //32 bytes
{
	/**/unsigned char DIR_Name[11]; /* File name */
	unsigned char DIR_Attr; /* File attributes */
	unsigned char DIR_NTRes; /* Reserved */
	unsigned char DIR_CrtTimeTenth; /* Created time (tenths of second) */
	unsigned short DIR_CrtTime; /* Created time (hours, minutes, seconds) */
	unsigned short DIR_CrtDate; /* Created day */
	unsigned short DIR_LstAccDate; /* Accessed day */
	/**/unsigned short DIR_FstClusHI; /* High 2 bytes of the first cluster address */
	unsigned short DIR_WrtTime; /* Written time (hours, minutes, seconds */
	unsigned short DIR_WrtDate; /* Written day */
	/**/unsigned short DIR_FstClusLO; /* Low 2 bytes of the first cluster address */
	/**/unsigned int/*long*/ DIR_FileSize; /* File size in bytes. (0 for directories) */
};
#pragma pack(pop)
/***********************************************************************/


/*****************Print out the usage of this tool**********************/
void printout_usage()
{
	printf("Usage: ./recover -d [device filename] [other arguments]\n");
	printf("-i                 Print boot sector information\n");
	printf("-l                 List all the directory entries\n");
	printf("-r filename        File recovery\n");
}
/***********************************************************************/


/*****************Print out the BOOT SECTOR Information*****************/
void print_bs_info(char *argv)
{
	FILE *ptr;
	if( (ptr = fopen(argv, "r")) )
	{
		struct BootEntry boot_entry;//if global then segmentation fault
									//can not use pointer, otherwise segmentation fault
		fread( &boot_entry, sizeof(struct BootEntry), 1, ptr );
		
		//printf("boot_entry->BPB_RootClus %lu\n", boot_entry.BPB_RootClus);
		printf("Number of FATs = %d\n", boot_entry.BPB_NumFATs);
		printf("Number of bytes per sector = %d\n", boot_entry.BPB_BytsPerSec);
		printf("Number of sectors per cluster = %d\n", boot_entry.BPB_SecPerClus);
		printf("Number of reserved sectors = %d\n", boot_entry.BPB_RsvdSecCnt);
		
		fclose(ptr);
	}
	else
	{
		printf("No such device\n");
	}
}
/***********************************************************************/


/********************List all Directory Entry***************************/
unsigned short Dir_name_mode( struct DirEntry dir_entry )//0 for filename, 1 for dirname, 2 for long filename
														//3 for deleted file, 4 for empty entry
{
	if( (dir_entry.DIR_Attr << 4) == 0xf0 )
	{
		return 2;// long file name
	}
	if( dir_entry.DIR_Name[0] == 0xe5 )
	{
		return 3;// deleted file name
	}
	if( (dir_entry.DIR_Attr << 3 >> 7) == 0x01 )
	{
		return 1;// directery name
	}
	int i;
	char keys[] = {'A','B','C','D','E','F','G','H','I','J','K',
				'L','M','N','O','P','Q','R','S','T','U','V','W','X',
				'Y','Z','1','2','3','4','5','6','7','8','9','0','$',
				'%','\'','\'','-','{','}','~','!','#','(',')','&','_','^'};
	for(i = 0; i < sizeof(keys); i++)
	{
		if(dir_entry.DIR_Name[0] == keys[i])
		{
			return 0;// valid file name
		}
	}
	return 4;
}

void print_name( struct DirEntry dir_entry, unsigned int mode )//0 mode for filename, 1 for directery, 2 for . and ..
{
	int i = 0;
	for(i = 0; i < 8; i++)
	{
		if(dir_entry.DIR_Name[i] != ' ')
		{
			printf("%c", dir_entry.DIR_Name[i]);
		}
	}
	for(i = 0; i < 3; i++)
	{
		if(dir_entry.DIR_Name[8+i] != ' ' && i == 0)
		{
			printf(".%c", dir_entry.DIR_Name[8+i]);
		}
		else if(dir_entry.DIR_Name[8+i] != ' ' && i != 0)
		{
			printf("%c", dir_entry.DIR_Name[8+i]);
		}
	}
	if( (mode == 1 && dir_entry.DIR_Name[0] != '.') || mode == 2 )
	{
		printf("/");
	}
	if( mode == 0 || mode == 1 )
	{
		printf(", %d, ", dir_entry.DIR_FileSize);
		printf("%d\n", ((unsigned int)(dir_entry.DIR_FstClusHI<<8)|dir_entry.DIR_FstClusLO));
	}
}

unsigned int/*long*/ count = 1;//must be global
void traverse_DirEntry(FILE *ptr, struct BootEntry boot_entry, struct DirEntry last_dir_entry, unsigned int mode,
					unsigned int/*long*/Cluster_Num)//, unsigned int/*long*/ count)// mode 1 for recursion
{
	unsigned int/*long*/ cluster2_offset = ((unsigned int)(boot_entry.BPB_BytsPerSec)) * 
			(((unsigned int)(boot_entry.BPB_RsvdSecCnt)) + 
			((unsigned int)(boot_entry.BPB_FATSz32)) * 
			((unsigned int)(boot_entry.BPB_NumFATs)));
	unsigned int/*long*/ FAT1_offset = ((unsigned int)(boot_entry.BPB_BytsPerSec)) * 
			((unsigned int)(boot_entry.BPB_RsvdSecCnt));
			
	fseek(ptr, cluster2_offset + (unsigned int)(boot_entry.BPB_BytsPerSec) * (unsigned int)(boot_entry.BPB_SecPerClus)
				* (Cluster_Num - 2), SEEK_SET);//go to cluster Cluster_Num
	struct DirEntry dir_entry;
	fread(&dir_entry, sizeof(struct DirEntry), 1, ptr);
	unsigned int dir_entry_count = 1;
	while( Dir_name_mode(dir_entry) != 4 )
	{
		unsigned int flag = Dir_name_mode(dir_entry);
		if( flag == 0 )
		{//valid file name, print it out
			printf("%d, ", count);
			if( mode == 1 )
			{
				print_name( last_dir_entry, 2 );
			}
			print_name(dir_entry, 0);
			count++;
		}
		if( flag == 1 )
		{//directery name
			if(dir_entry.DIR_Name[0] != '.')
			{
				printf("%d, ", count);
				print_name(dir_entry, 1);
				count++;

				long temp = ftell(ptr);//
				unsigned int a = (unsigned int)((dir_entry.DIR_FstClusHI<<16)|dir_entry.DIR_FstClusLO);
				traverse_DirEntry(ptr, boot_entry, dir_entry, 1, a);//, count);
				fseek(ptr, temp, SEEK_SET);//
			}
			else
			{
				printf("%d, ", count);
				print_name(last_dir_entry, 2);
				print_name(dir_entry, 1);
				count++;
			}
		}
		if( flag == 2 )
		{//long file name unused part, skip it
		}
		if( flag == 3 )
		{//deleted file, skip it
		}
		
		if( dir_entry_count == (unsigned int)(boot_entry.BPB_BytsPerSec) * 
						(unsigned int)(boot_entry.BPB_SecPerClus) / sizeof(struct DirEntry) )
		{//move to the next cluster if exist
			//printf("%d\n", dir_entry_count);
			fseek(ptr, FAT1_offset + Cluster_Num * sizeof(int/*long*/), SEEK_SET);
			unsigned int/*long*/ FAT_pointer;
			fread(&FAT_pointer, sizeof(int/*long*/), 1, ptr);
			if( 0x0ffffff8 <= (FAT_pointer<<4>>4) && (FAT_pointer<<4>>4) <= 0x0fffffff)//EOF
			{
				break;
			}
			else if( 0x000000002 <= (FAT_pointer<<4>>4) && (FAT_pointer<<4>>4) <= 0x0ffffffef )
			{
				dir_entry_count = 0;
				traverse_DirEntry(ptr, boot_entry, last_dir_entry, mode, FAT_pointer);
			}
		}
		
		//read an other one dir_entry
		//printf("%d\n", dir_entry_count);
		dir_entry_count++;
		fread(&dir_entry, sizeof(struct DirEntry), 1, ptr);
	}
}

void list_all_direntry(char *argv)
{
	//starting index of FAT1 is (number_of_reserve_sector * bytes_per_sector) + 1
	//FAT can be viewed as an unsigned integer array
	//each entry is a 32-bit unsigned integer
	FILE *ptr;
	if( (ptr = fopen(argv, "r")) )
	{
		struct BootEntry boot_entry;//if global then segmentation fault
		fread( &boot_entry, sizeof(struct BootEntry), 1, ptr );
		struct DirEntry TEMP;
		traverse_DirEntry(ptr, boot_entry, TEMP, 0, 2);//, 1);

		fclose(ptr);
	}
	else
	{
		printf("No such device\n");
	}
}
/***********************************************************************/


/****************************Recover File*******************************/
unsigned int success = 0;
unsigned int compare(char *argv, struct DirEntry dir_entry)
{
	int i = 0;
	int k = 0;
	char temp[13];
	temp[k++] = argv[0];
	for(i = 1; i < 11; i++)
	{
		if(i != 8 && dir_entry.DIR_Name[i] != ' ')
		{
			temp[k++] = dir_entry.DIR_Name[i];
		}
		if( i == 8 && dir_entry.DIR_Name[i] != ' ' )
		{
			temp[k++] = '.';
			temp[k++] = dir_entry.DIR_Name[i];
		}
	} 
	temp[k] = '\0';
	if( !strcmp(argv, temp) ){return 1;}
	return -1;
}

void find_and_change(char *argv, FILE *ptr, struct BootEntry boot_entry, struct DirEntry last_dir_entry, unsigned int mode,
					unsigned int/*long*/Cluster_Num)
{
	unsigned int/*long*/ cluster2_offset = ((unsigned int)(boot_entry.BPB_BytsPerSec)) * 
			(((unsigned int)(boot_entry.BPB_RsvdSecCnt)) + 
			((unsigned int)(boot_entry.BPB_FATSz32)) * 
			((unsigned int)(boot_entry.BPB_NumFATs)));
	unsigned int/*long*/ FAT1_offset = ((unsigned int)(boot_entry.BPB_BytsPerSec)) * 
			((unsigned int)(boot_entry.BPB_RsvdSecCnt));
			
	fseek(ptr, cluster2_offset + (unsigned int)(boot_entry.BPB_BytsPerSec) * (unsigned int)(boot_entry.BPB_SecPerClus)
				* (Cluster_Num - 2), SEEK_SET);//go to cluster Cluster_Num
	
	struct DirEntry dir_entry;
	fread(&dir_entry, sizeof(struct DirEntry), 1, ptr);
	unsigned int dir_entry_count = 1;
	while( Dir_name_mode(dir_entry) != 4 )
	{
		unsigned int flag = Dir_name_mode(dir_entry);
		if( flag == 0 )
		{//valid file name, skip it
		}
		if( flag == 1 )
		{//directery name
			if(dir_entry.DIR_Name[0] != '.')
			{
				long temp = ftell(ptr);//
				unsigned int a = (unsigned int)((dir_entry.DIR_FstClusHI<<16)|dir_entry.DIR_FstClusLO);//go to first cluster of directery
				find_and_change(argv, ptr, boot_entry, dir_entry, 1, a);
				fseek(ptr, temp, SEEK_SET);//
			}
			else
			{//no used, skip it
			}
		}
		if( flag == 2 )
		{//long file name unused part, skip it
		}
		if( flag == 3 )
		{//deleted file
			if( compare(argv, dir_entry) == 1 )//delete filename is found
			{
				unsigned int cluster_bytes = (unsigned int)(boot_entry.BPB_BytsPerSec) * (unsigned int)(boot_entry.BPB_SecPerClus);
				//first find out the first cluster and how many cluster it used
				unsigned int first_cluster_num = (unsigned int)((dir_entry.DIR_FstClusHI<<16)|dir_entry.DIR_FstClusLO);
				unsigned int cluster_dev = dir_entry.DIR_FileSize / cluster_bytes;
				unsigned int cluster_mod = dir_entry.DIR_FileSize % cluster_bytes;
				unsigned int num_of_cluster;
				if(cluster_mod == 0){num_of_cluster = cluster_dev;}
				else{num_of_cluster = cluster_dev + 1;}
				
				//check in FATs whether the cluster has been occupied
				long temp = ftell(ptr);//
				fseek(ptr, FAT1_offset + first_cluster_num * sizeof(int/*long*/), SEEK_SET);//go to the FAT[first cluster]
				int FAT_array[num_of_cluster];//???????????????????????????????
				fread(FAT_array, sizeof(FAT_array), 1, ptr);
				int ct;
				int flag = 1;
				for(ct = 0; ct < num_of_cluster; ct++)
				{
					if(FAT_array[ct] != 0)
					{
						printf("%s: error - fail to recover\n", argv);
						flag = 0;
						exit(0);
						break;
					}
				}
				
				//if not being occupied, recover
				if( flag == 1 )
				{
					success = 1;
					long dir_name = temp - sizeof(struct DirEntry);
					fseek(ptr, dir_name, SEEK_SET);
					char a = argv[0];
					fwrite(&a, 1, 1, ptr);
					fseek(ptr, temp, SEEK_SET);//
					
					//chang the FAT_array
					if( num_of_cluster >=1 )//special case for empty file, num_of_cluster = 0
					{
						unsigned int fcn_temp = first_cluster_num;
						int dt;
						for(dt = 0; dt < num_of_cluster - 1; dt++)
						{
							FAT_array[dt] = ++fcn_temp;
						}
					
						FAT_array[num_of_cluster - 1] = 0x0fffffff;
					}
					
					//write the FAT_array into FS
					fseek(ptr, FAT1_offset + first_cluster_num * sizeof(int/*long*/), SEEK_SET);//go to the FAT[first cluster]
					if( num_of_cluster >=1 )//special case for empty file, num_of_cluster = 0
					{
						fwrite(FAT_array, sizeof(FAT_array), 1, ptr);
					}
					
					printf("%s: recovered in /", argv);
					if(mode == 1)
					{
						int et;
						for(et = 0; et < 8; et++)
						{
							if(last_dir_entry.DIR_Name[et] != ' ')
							{
								printf("%c", last_dir_entry.DIR_Name[et]);
							}
						}
					}
					printf("\n");	
				}	
				fseek(ptr, temp, SEEK_SET);//
				//break;
			}
			else
			{//do nothing
			}
		}
		
		if( dir_entry_count == (unsigned int)(boot_entry.BPB_BytsPerSec) * 
						(unsigned int)(boot_entry.BPB_SecPerClus) / sizeof(struct DirEntry) )
		{//move to the next cluster if exist
			//printf("%d\n", dir_entry_count);
			fseek(ptr, FAT1_offset + Cluster_Num * sizeof(int/*long*/), SEEK_SET);
			unsigned int/*long*/ FAT_pointer;
			fread(&FAT_pointer, sizeof(int/*long*/), 1, ptr);
			if( 0x0ffffff8 <= (FAT_pointer<<4>>4) && (FAT_pointer<<4>>4) <= 0x0fffffff)//EOF
			{
				break;
			}
			else if( 0x000000002 <= (FAT_pointer<<4>>4) && (FAT_pointer<<4>>4) <= 0x0ffffffef )
			{
				dir_entry_count = 0;
				find_and_change(argv, ptr, boot_entry, last_dir_entry, mode, FAT_pointer);
			}
		}
		
		//read an other one dir_entry
		//printf("%d\n", dir_entry_count);
		dir_entry_count++;
		fread(&dir_entry, sizeof(struct DirEntry), 1, ptr);
	}
	
	if( mode == 0 && success == 0)//at last file not found
	{
		printf("%s: error - file not found\n", argv);
	}
}

void recover(char *device_name, char *recover_filename)
{
	//starting index of FAT1 is (number_of_reserve_sector * bytes_per_sector) + 1
	//FAT can be viewed as an unsigned integer array
	//each entry is a 32-bit unsigned integer
	FILE *ptr;
	if( (ptr = fopen(device_name, "r+")) )
	{
		struct BootEntry boot_entry;//if global then segmentation fault
		fread( &boot_entry, sizeof(struct BootEntry), 1, ptr );
		struct DirEntry TEMP;
		find_and_change(recover_filename, ptr, boot_entry, TEMP, 0, 2);//, 1);
		fclose(ptr);
	}
	else
	{
		printf("No such device\n");
	}
}
/***********************************************************************/

int main(int argc, char **argv)
{
	if( argc == 4 || argc == 5 )
	{
		if( !strcmp(argv[1], "-d") && !strcmp(argv[3], "-i") && argc == 4)
		{
			//printf("Print boot sector information\n");
			print_bs_info(argv[2]);
		}
		else if( !strcmp(argv[1], "-d") && !strcmp(argv[3], "-l") && argc == 4 )
		{
			//printf("List all the directory entries\n");
			list_all_direntry(argv[2]);
		}
		else if( !strcmp(argv[1], "-d") && !strcmp(argv[3], "-r") && argc == 5 )
		{
			//printf("File recovery\n");
			recover(argv[2], argv[4]);
		}
		else
		{
			printout_usage();
		}
	}
	else
	{
		printout_usage();
	}
	
	exit(0);
}
