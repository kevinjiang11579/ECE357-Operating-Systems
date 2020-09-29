#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

void readDirectory(char *dName);

int main(int argc, char *argv[])
{
	readDirectory(argv[1]);
}

void readDirectory(char *dName)
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
				readDirectory(nextPath);
			}
		}
		else
		{
			printf("Name of file: %s, file type: %d\n", de->d_name, de->d_type);
		}
	}
	closedir(dp);
	return;
}
