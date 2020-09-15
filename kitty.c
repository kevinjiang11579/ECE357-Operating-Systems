/*ECE357 Assignment 1, Kevin Jiang*/
#include <stdio.h>

int main(int argc, char *argv[])
{
	char buf[4096];
	int lim;
	int readResult;
	int writeResult;
	lim = sizeof(buf)
	if(argc == 1)
	{
		printf("Reading from stdin, printing to stdout\n");
		readResult = read(0, buf, lim);
		if(readResult == 0)
		{
			printf("End of file reached\n");
			writeResult = write(1, buf, lim);
			printf("%d bytes written\n", writeResult);
		}
		else if(readResult < 0)
		{
			printf("Error occured when reading file\n");
			return -1;
		}
		else
		{
			printf("%d bytes read\n", readResult);
		}

	}
	return 0;
}
