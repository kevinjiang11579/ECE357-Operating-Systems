/*ECE357 Assignment 1, Kevin Jiang*/
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
int main(int argc, char *argv[])
{
	char buf[4096];
	int lim, readResult, writeResult, getoptResult, fdIn, fdOut, closeResult;
	lim = sizeof(buf);
	char *outName = NULL;
	char *inName = NULL;
	char *currentArg = NULL;
	extern char *optarg;
	extern int optind, opterr, optopt;
	_Bool outExist = 0;
	fdOut = STDOUT_FILENO;
	fdIn = STDIN_FILENO;
	while((getoptResult = getopt(argc, argv, "o:")) != -1)
	{
		switch (getoptResult)
		{
			case 'o':
				if(outExist)
				{
					printf("Please specify one output file\n");
					return -1;
				}
				outName = optarg;
				printf("Output file name is %s\n", outName);
				fdOut = open(outName, O_WRONLY|O_CREAT|O_APPEND, 0666);
				outExist = 1;
				break;
		}
	}
	if(optind == 1)
	{
		printf("No output file specified, printing to stdout\n");
	}
	if(optind == argc)
	{
		printf("No input file specifed, reading from stdin\n");
		while((readResult = read(fdIn, buf, lim)) != 0)
		{
			if(readResult < 0)
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
		printf("End of file reached\n");
	}
	else
	{
		for(int argIndex = optind; argIndex < argc; argIndex++)
		{
			currentArg = argv[argIndex];
			if(currentArg[0] == '-')
			{
				printf("Reading input from stdin\n");
				fdIn = STDIN_FILENO;
			}
			else
			{
				inName = currentArg;
				printf("Input file name is: %s\n", inName);
				fdIn = open(inName, O_RDONLY);
			}
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
			}
			writeResult = write(fdOut, buf, readResult);
			printf("%d bytes written\n", writeResult);
			if(fdIn > 0){closeResult = close(fdIn);}
		}
	}
	return 0;
}
