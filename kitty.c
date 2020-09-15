/*ECE357 Assignment 1, Kevin Jiang*/
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
	char buf[4096];
	int lim;
	int readResult;
	int writeResult;
	lim = sizeof(buf);
	if(argc == 1)
	{
		printf("Reading from stdin, printing to stdout\n");
		readResult = read(STDIN_FILENO, buf, lim);
		if(readResult == 0)
		{
		}
		else if(readResult < 0)
		{
			printf("Error occured when reading file\n");
			return -1;
		}
		else
		{
			printf("%d bytes read\n", readResult);
			writeResult = write(STDOUT_FILENO, buf, readResult);
			printf("%d bytes written\n", writeResult);
		}

	}
	else
	{
	}
	return 0;
}
