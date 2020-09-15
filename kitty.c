/*ECE357 Assignment 1, Kevin Jiang*/
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
	char buf[4096];
	int lim, readResult, writeResult, getoptResult, fdIn, fdOut, closeResult, bytesWritten, countRW, partialWriteResult;
	lim = sizeof(buf);
	char *outName = "<standard output>", *inName = "<standard input>", *currentArg = NULL;
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
	if(optind == argc) //No input files specified
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
				partialWriteResult = 0;
				while((writeResult = write(fdOut, buf, readResult)) < readResult) //In case of partial write, try to write again after increasing buffer pointer
				{
					if(writeResult < 0)
					{
						printf("Error occured when writing to %s, errno = %d, %s\n", outName, errno, strerror(errno));
					}
					partialWriteResult = writeResult;
					buf += partialWriteResult;
				}
				buf -= partialWriteResult;
				bytesWritten += writeResult;
				countRW += 1;
			}
			if(isBinary){printf("WARNING: input file is a binary file\n");}
			printf("Input file: %s, Total bytes written: %d, Read/Writes made: %d\n", inName, bytesWritten, countRW);
		}
	}
	else //Input files are specified
	{
		for(int argIndex = optind; argIndex < argc; argIndex++)
		{
			bytesWritten = 0;
			countRW = 0;
			currentArg = argv[argIndex];
			if(currentArg[0] == '-') //Input fron stdin
			{
				fdIn = STDIN_FILENO;
				inName = "<standard input>";
			}
			else //Input from a file
			{
				inName = currentArg;
				fdIn = open(inName, O_RDONLY);
			}
			while((readResult = read(fdIn, buf, lim)) != 0)
			{
				for(int i = 0; i < readResult; i++) //Check if input file is a binary file
				{
					if(!(isspace(buf[i])||isprint(buf[i])))
					{
						isBinary = 1;
						break;
					}
				}
				if(readResult < 0)
				{
					printf("Error occured when reading from %s, errno = %d, %s\n", inName, errno, strerror(errno));
					return -1;
				}
				partialWriteResult = 0;
				while((writeResult = write(fdOut, buf, readResult)) < readResult); //In case of partial write, try to write again after increasing buffer pointer
				{
					if(writeResult < 0)
					{
						printf("Error occured when writing to %s, errno = %d, %s\n", outName, errno, strerror(errno));
					}
					partialWriteResult = writeResult;
					buf += partialWriteResult;
				}
				buf -= partialWriteResult;
				bytesWritten += writeResult;
				countRW += 1;
			}
			if(isBinary){printf("WARNING: input file is a binary file\n");}
			printf("Input file: %s, Total bytes written: %d, Read/Writes made: %d\n", inName, bytesWritten, countRW);
			if(fdIn != STDIN_FILENO)
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
	if(fdOut != STDOUT_FILENO)
	{
		closeResult = close(fdOut);
		if(closeResult != 0)
		{
			printf("Error occured when closing %s, errno = %d, %s\n", outName, errno, strerror(errno));
			return -1;
		}
	}
	return 0;
}
