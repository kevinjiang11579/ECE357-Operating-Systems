#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define TYPE_NUM 16

void readDirectory(char *dName, int *fileTypeCount);

int main(int argc, char *argv[])
{
	int fileTypeCount[TYPE_NUM];
	for(int i = 0; i < TYPE_NUM; i++)
	{
		fileTypeCount[i] = 0;
	}
	readDirectory(argv[1], fileTypeCount);
	for(int i = 0; i < TYPE_NUM; i++)
	{
		if(fileTypeCount[i] > 0)
		{
			printf("Number of inodes of type %d: %d\n", i, fileTypeCount[i]);
		}
	}
}

void readDirectory(char *dName, int *fileTypeCount)
{
	DIR *dp;
	struct dirent *de;
	char nextPath[256];
	char startPath[256];
	strcpy(startPath, dName);
	printf("Directory Name: %s\n", startPath);
	if(!(dp = opendir(dName)))
	{
		fprintf(stderr,"Cannot open directory %s, errno: %d, %s\n", dName, errno, strerror(errno));
		return;	
	}
	errno = 0;
	while(de = readdir(dp))
	{
		if(de->d_type == DT_DIR)
		{
 			if(strcmp(de->d_name,".") != 0 && strcmp(de->d_name, "..") != 0)
			{
				strcpy(nextPath, dName);
				strcat(nextPath, "/");
				strcat(nextPath, de->d_name);
				printf("Next Path: %s\n", nextPath);
				readDirectory(nextPath, fileTypeCount);
			}
		}
		else
		{
			printf("Name of file: %s, file type: %d\n", de->d_name, de->d_type);
		}
		fileTypeCount[de->d_type] += 1;
	}
	closedir(dp);
	return;
}
