#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
	if((symlink(argv[1], argv[2])) == -1);
	{
		fprintf(stderr, "Failed to create symbolic link: %s", strerr(errno));
	}

}