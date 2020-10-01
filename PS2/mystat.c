#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define NUM_TYPES 7

enum typeindex{sock_type, symlink_type, reg_type, blkdev_type, dir_type, chardev_type, fifo_type};

void readDirectory(char *dName, int *fileTypeCount, int *totalBlocks, int *totalSize, int *multiLinkCount, int *badSymLinkCount);

int main(int argc, char *argv[])
{
	int fileTypeCount[NUM_TYPES];
	int totalBlocks = 0;
	int totalSize = 0;
	int multiLinkCount = 0;
	int badSymLinkCount = 0;
	char *typeNames[NUM_TYPES] = {[sock_type] = "Sock", [symlink_type] = "Symbolic Link",
								[reg_type] = "File", [blkdev_type] = "Block Device", [dir_type] = "Directory",
								[chardev_type] = "Character Device", [fifo_type] = "FIFO"};
	for(int i = 0; i < NUM_TYPES; i++) // Set the array to 0 first
	{
		fileTypeCount[i] = 0;
	}

	readDirectory(argv[1], fileTypeCount, &totalBlocks, &totalSize, &multiLinkCount, &badSymLinkCount);

	printf("------------------------------\nNumber of inodes of each type\n");
	for(int i = 0; i < NUM_TYPES; i++)
	{
		if(fileTypeCount[i] > 0)
		{
			printf("%s: %d\n", typeNames[i], fileTypeCount[i]);
		}
	}
	printf("------------------------------\n");
	printf("Sum of file sizes: %d\nSum of blocks allocated: %d\nRatio of Size to Blocks: %d\n", totalSize, totalBlocks, totalSize/totalBlocks);
	printf("------------------------------\n");
	printf("Number of inodes with link count of more than 1: %d\n", multiLinkCount);
	printf("------------------------------\n");
	printf("Number of symbolic links that did not resolve to a valid target: %d\n", badSymLinkCount);
	return 0;
}

void readDirectory(char *dName, int *fileTypeCount, int *totalBlocks, int *totalSize, int *multiLinkCount, int *badSymLinkCount)
{
	DIR *dp;
	struct dirent *de;
	struct stat st;
	char filePath[256];
	int fd;

	//printf("Directory Name: %s\n", dName);
	if(!(dp = opendir(dName)))
	{
		fprintf(stderr,"Cannot read directory %s, %s. Skipping directory.\n", dName, strerror(errno));
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
				//printf("Next Path: %s\n", filePath);
				fileTypeCount[dir_type] += 1;
				readDirectory(filePath, fileTypeCount, totalBlocks, totalSize, multiLinkCount, badSymLinkCount);
			}
		}
		else
		{
			*multiLinkCount += st.st_nlink > 1;
			//printf("Name of file: %s, file type: %d\n", de->d_name, de->d_type);
			if((st.st_mode & S_IFMT) == S_IFREG)
			{
				fileTypeCount[reg_type] += 1;
				if((fd = open(filePath, O_RDONLY)) == -1)
				{
					fprintf(stderr, "Cannot open file %s for reading, %s. Skipping file.\n", filePath, strerror(errno));
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
				//printf("Symbolic link found!\n");
				if((fd = open(filePath,O_RDONLY)) == -1)
				{
					//printf("This symbolic link is hanging\n");
					*badSymLinkCount += 1;
				}
				close(fd);
			}

		}
	}
	closedir(dp);
	return;
}
