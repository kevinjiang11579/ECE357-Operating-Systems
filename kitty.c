/*ECE357 Assignment 1, Kevin Jiang*/
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
int main(int argc, char *argv[])
{
	char buf[4096];
	int lim, readResult, writeResult, getoptResult;
	lim = sizeof(buf);
	char *outName = NULL;
	extern char *optarg;
	extern int optind, opterr, optopt;
	if(argc == 1)
	{
		printf("Reading from stdin, printing to stdout\n");
		readResult = read(STDIN_FILENO, buf, lim);
		if(readResult == 0)
		{
			printf("End of file reached\n");
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
		while((getoptResult = getopt(argc, argv, "o:")) != -1)
		{
			switch (getoptResult)
			{
				case 'o':
					outName = optarg;
					printf("Output file name is %s", outName);
					break;
			}
		}
		printf("optind debug: %d", optind);
	}
	return 0;
}
