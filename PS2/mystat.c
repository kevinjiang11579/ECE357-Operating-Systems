#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define TYPE_NUM 16

void readDirectory(char *dName, int *fileTypeCount, int *totalBlocks, int *totalSize, int *multiLinkCount);

int main(int argc, char *argv[])
{
	int fileTypeCount[TYPE_NUM];
	int totalBlocks = 0;
	int totalSize = 0;
	int multiLinkCount = 0;
	for(int i = 0; i < TYPE_NUM; i++) // Set the array to 0 first
	{
		fileTypeCount[i] = 0;
	}
	readDirectory(argv[1], fileTypeCount, &totalBlocks, &totalSize, &multiLinkCount);
	for(int i = 0; i < TYPE_NUM; i++)
	{
		if(fileTypeCount[i] > 0)
		{
			printf("Number of inodes of type %d: %d\n", i, fileTypeCount[i]);
		}
	}
	printf("Sum of file sizes: %d, Sum of blocks allocated: %d\n", totalSize, totalBlocks);

}

void readDirectory(char *dName, int *fileTypeCount, int *totalBlocks, int *totalSize, int *multiLinkCount)
{
	DIR *dp;
	struct dirent *de;
	struct stat st;
	char filePath[256];
	int fd;

	printf("Directory Name: %s\n", dName);
	if(!(dp = opendir(dName)))
	{
		fprintf(stderr,"Cannot open directory %s, errno: %d, %s\n", dName, errno, strerror(errno));
		return;	
	}
	errno = 0;
	while(de = readdir(dp))
	{
		strcpy(filePath, dName);
		strcat(filePath, "/");
		strcat(filePath, de->d_name);
		if(de->d_type == DT_DIR)
		{
 			if(strcmp(de->d_name,".") != 0 && strcmp(de->d_name, "..") != 0)
			{
				printf("Next Path: %s\n", filePath);
				readDirectory(filePath, fileTypeCount, totalBlocks, totalSize, multiLinkCount);
			}
			else
			{
				stat(filePath, &st);
			}
		}
		else
		{
			if(de->d_type = DT_REG)
			{
				printf("Name of file: %s, file type: %d\n", de->d_name, de->d_type);
				fd = open(filePath, O_RDONLY);
				fstat(fd, &st);
				*totalBlocks += st.st_blocks;
				*totalSize += st.st_size;
				close(fd);
			}
			else
			{
			stat(filePath, &st);
				if(st.st_nlink > 1)
				{
					*multiLinkCount += 1;
				}
			}

		}
		fileTypeCount[de->d_type] += 1;
	}
	closedir(dp);
	return;
}
