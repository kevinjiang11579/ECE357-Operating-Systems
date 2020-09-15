/*ECE357 Assignment 1, Kevin Jiang*/
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char buf[4096];
	int lim, readResult, writeResult, getoptResult, fdIn, fdOut, closeResult, bytesWritten, countRW;
	lim = sizeof(buf);
	char *outName = NULL;
	char *inName = NULL;
	char *currentArg = NULL;
	extern char *optarg;
	extern int optind, opterr, optopt;
	_Bool outExist = 0;
	_Bool isBinary = 0;
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
		readResult = read(fdIn, buf, lim);
		if(readResult < 0)
		{
			printf("Error occured when reading file, errno: %s\n", strerror(errno));
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
				fdIn = STDIN_FILENO;
			}
			else
			{
				inName = currentArg;
				printf("Input file name is: %s\n", inName);
				fdIn = open(inName, O_RDONLY);
			}
			while((readResult = read(fdIn, buf, lim)) != 0)
			{
				for(int i = 0; i < readResult; i++)
				{
					if(buf[i] < 32 && buf[i] != 10)
					isBinary = 1;
					break;
				}
				if(readResult < 0)
				{
					printf("Error occured when reading file, errno: %s\n",strerror(errno));
					return -1;
				}
				else
				{
					printf("%d bytes read\n", readResult);
				}
			writeResult = write(fdOut, buf, readResult);
			bytesWritten += writeResult;
			countRW += 1;
			}
			printf("End of file reached\n");
			if(isBinary){printf("WARNING: input file is a binary file\n");}
			if(inName == STDIN_FILENO){inName = "<standard input>";};
			printf("Input file: %s, Total bytes written: %d, Read/Writes made: %d\n", inName, bytesWritten, countRW);
			if(fdIn > 0)
			{
				closeResult = close(fdIn);
				if(closeResult != 0)
				{
					printf("Error occured when closing file, errno: %s\n", strerror(errno));
					return -1;
				}
				else{printf("File successfully closed\n");}
			}
		}
	}
	return 0;
}
