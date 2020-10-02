#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdbool.h>

#define NUM_TYPES 7

enum typeindex{sock_type, symlink_type, reg_type, blkdev_type, dir_type, chardev_type, fifo_type};

void readDirectory(char *dName, int *fileTypeCount, int *totalBlocks, int *totalSize, int *multiLinkCount, int *badSymLinkCount, int *badEntryNameCount);
void recordFileStats(char *filePath, int *fileTypeCount, int *totalBlocks, int *totalSize, int *multiLinkCount, int *badSymLinkCount, struct stat st);
bool isProblematic(char *entryName);

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Error: Less than one or more than one file system path specified. Please specify only one.\n");
		return -1;
	}
	if(strstr(argv[1], "/proc"))
	{
		fprintf(stderr, "Error: This program cannot accept an argument that contains the /proc directory.\n"); //You mentioned in class not to go into proc
		return -1;
	}
	int fileTypeCount[NUM_TYPES];
	int totalBlocks = 0;
	int totalSize = 0;
	int ratio = 0;
	int multiLinkCount = 0;
	int badSymLinkCount = 0;
	int badEntryNameCount = 0;
	char *typeNames[NUM_TYPES] = {[sock_type] = "Sock", [symlink_type] = "Symbolic Link",
								[reg_type] = "File", [blkdev_type] = "Block Device", [dir_type] = "Directory",
								[chardev_type] = "Character Device", [fifo_type] = "FIFO"};
	struct stat st;

	for(int i = 0; i < NUM_TYPES; i++) // Set the array to 0 first
	{
		fileTypeCount[i] = 0;
	}

	lstat(argv[1], &st);
	if((st.st_mode & S_IFMT) == S_IFDIR)
	{
		readDirectory(argv[1], fileTypeCount, &totalBlocks, &totalSize, &multiLinkCount, &badSymLinkCount, &badEntryNameCount);
	}
	else //if the argument is a file, then just give stats for that single file
	{
		recordFileStats(argv[1], fileTypeCount, &totalBlocks, &totalSize, &multiLinkCount, &badSymLinkCount, st);
		badEntryNameCount += isProblematic(argv[1]);
	}

	printf("------------------------------\nNumber of inodes of each type\n");
	for(int i = 0; i < NUM_TYPES; i++)
	{
		if(fileTypeCount[i] > 0)
		{
			printf("%s: %d\n", typeNames[i], fileTypeCount[i]);
		}
	}
	printf("------------------------------\n");
	if(totalBlocks)
	{
		ratio = totalSize/totalBlocks;
		printf("Sum of file sizes: %d\nSum of blocks allocated: %d\nRatio of Size to Blocks: %d\n", totalSize, totalBlocks, ratio);
	}
	else {printf("Sum of file sizes: %d\nSum of blocks allocated: %d\nRatio of Size to Blocks: Undefined\n", totalSize, totalBlocks);}
	printf("------------------------------\n");
	printf("Number of inodes with link count of more than 1: %d\n", multiLinkCount);
	printf("------------------------------\n");
	printf("Number of symbolic links that did not resolve to a valid target: %d\n", badSymLinkCount);
	printf("------------------------------\n");
	printf("Number of entries with \"problematic\" names: %d\n", badEntryNameCount);
	return 0;
}

void readDirectory(char *dName, int *fileTypeCount, int *totalBlocks, int *totalSize, int *multiLinkCount, int *badSymLinkCount, int *badEntryNameCount)
{
	DIR *dp;
	struct dirent *de;
	struct stat st;
	char filePath[256];

	if(!(dp = opendir(dName)))
	{
		fprintf(stderr,"Cannot read directory %s, %s. Moving on to next entry.\n", dName, strerror(errno));
		return;	
	}
	errno = 0;
	while(de = readdir(dp))
	{
		strcpy(filePath, dName);
		strcat(filePath, "/");
		strcat(filePath, de->d_name);
		lstat(filePath, &st);
		if((st.st_mode & S_IFMT) == S_IFDIR)
		{
 			if(strcmp(de->d_name,".") != 0 && strcmp(de->d_name, "..") != 0 && strcmp(de->d_name, "proc") != 0)
			{
				fileTypeCount[dir_type] += 1;
				readDirectory(filePath, fileTypeCount, totalBlocks, totalSize, multiLinkCount, badSymLinkCount, badEntryNameCount);
			}
		}
		else
		{
			recordFileStats(filePath, fileTypeCount, totalBlocks, totalSize, multiLinkCount, badSymLinkCount, st);
		}
		*badEntryNameCount += isProblematic(de->d_name);

	}
	closedir(dp);
	return;
}

void recordFileStats(char *filePath, int *fileTypeCount, int *totalBlocks, int *totalSize, int *multiLinkCount, int *badSymLinkCount, struct stat st)
{
	int fd;

	*multiLinkCount += st.st_nlink > 1;
	if((st.st_mode & S_IFMT) == S_IFREG)
	{
		fileTypeCount[reg_type] += 1;
		if((fd = open(filePath, O_RDONLY)) == -1)
		{
			fprintf(stderr, "Cannot open file %s for reading, %s. Moving on to next entry.\n", filePath, strerror(errno));
		}
		else
		{
			fstat(fd, &st);
			*totalBlocks += st.st_blocks;
			*totalSize += st.st_size;
			close(fd);
		}
	}
	else if((st.st_mode & S_IFMT) == S_IFLNK)
	{
		fileTypeCount[symlink_type] += 1;
		if((fd = open(filePath,O_RDONLY)) == -1)
		{
			*badSymLinkCount += 1;
		}
		close(fd);
	}	
}

bool isProblematic(char *entryName)
{
	for(int i = 0; i < strlen(entryName); i++)
	{
		if(!isprint(entryName[i]) || !isascii(entryName[i]) || iscntrl(entryName[i]))
		{
			return 1;
		}
	}
	return 0;
}