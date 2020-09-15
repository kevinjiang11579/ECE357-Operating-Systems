/*ECE357 Assignment 1, Kevin Jiang*/
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
int main(int argc, char *argv[])
{
	char buf[4096];
	int lim, readResult, writeResult, getoptResult, fdIn, fdOut;
	lim = sizeof(buf);
	char *outName = NULL;
	char *inName = NULL;
	char *currentArg = NULL;
	extern char *optarg;
	extern int optind, opterr, optopt;
	fdOut = STDOUT_FILENO;
	fdIn = STDIN_FILENO;
	while((getoptResult = getopt(argc, argv, "o:")) != -1)
	{
		switch (getoptResult)
		{
			case 'o':
				outName = optarg;
				printf("Output file name is %s\n", outName);
				break;
		}
	}
	if(optind == 1)
	{
		printf("No output file specified, printing to stdout\n");
	}
	if(argc == optind)
	{
		printf("No input file specifed, reading from stdin\n");
		readResult = read(fdIn, buf, lim);
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
			writeResult = write(fdOut, buf, readResult);
			printf("%d bytes written\n", writeResult);
		}
	}
	else
	{
		for(int argIndex = optind; argIndex < argc; argIndex++)
		{
			currentArg = argv[argIndex];
			if(currentArg[0] == '-')
			{
				printf("Reading input from stdin\n");
			}
			else
			{
				inName = currentArg;
				printf("Input file name is: %s\n", inName);
			}
		}
	}
	return 0;
}
