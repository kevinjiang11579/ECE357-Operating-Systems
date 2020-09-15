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
	char *outName = NULL, *inName = "<standard input>", *currentArg = NULL;
	extern char *optarg;
	extern int optind, opterr, optopt;
	_Bool outExist = 0, isBinary = 0;
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
				fdOut = open(outName, O_WRONLY|O_CREAT|O_APPEND, 0666);
				outExist = 1;
				break;
		}
	}
	if(optind == argc)
	{
		while((readResult = read(fdIn, buf, lim)) != 0);
		{
			if(readResult < 0)
			{
				printf("Error occured when reading from %s, errno = %d, %s\n", inName, errno, strerror(errno));
				return -1;
			}
			else
			{
				writeResult = write(fdOut, buf, readResult);
				bytesWritten += writeResult;
				countRW += 1;
			}
		}
	}
	else
	{
		for(int argIndex = optind; argIndex < argc; argIndex++)
		{
			currentArg = argv[argIndex];
			if(currentArg[0] == '-')
			{
				fdIn = STDIN_FILENO;
				inName = "<standard input>";
			}
			else
			{
				inName = currentArg;
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
					printf("Error occured when reading %s, errno = %d, %s\n", inName, errno, strerror(errno));
					return -1;
				}
				writeResult = write(fdOut, buf, readResult);
				bytesWritten += writeResult;
				countRW += 1;
			}
			if(isBinary){printf("WARNING: input file is a binary file\n");}
			printf("Input file: %s, Total bytes written: %d, Read/Writes made: %d\n", inName, bytesWritten, countRW);
			if(fdIn > 0)
			{
				closeResult = close(fdIn);
				if(closeResult != 0)
				{
					printf("Error occured when closing %s, errno = %d, %s\n", inName, errno, strerror(errno));
					return -1;
				}
			}
		}
	}
	return 0;
}
