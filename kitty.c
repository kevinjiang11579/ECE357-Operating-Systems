/*ECE357 Assignment 1, Kevin Jiang*/
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

char buffer[4096];
char *buf = buffer;
int lim = 4096, readResult, writeResult, getoptResult, fdIn, fdOut, closeResult, bytesWritten, countRW, partialWriteResult;
char *outName = "<standard output>", *inName = "<standard input>", *currentArg = NULL;
extern char *optarg;
extern int optind, opterr, optopt;
_Bool outExist = 0, isBinary = 0;

int readWrite()
{
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
	return 0;
}

int main(int argc, char *argv[])
{
	fdOut = STDOUT_FILENO;
	fdIn = STDIN_FILENO;
	while((getoptResult = getopt(argc, argv, "o:")) != -1)
	{
		switch (getoptResult)
		{
			case 'o':
				if(outExist)
				{
					printf("Error: Please specify only one output file.\n");
					return -1;
				}
				outName = optarg;
				if((fdOut = open(outName, O_WRONLY|O_CREAT|O_APPEND, 0666)) == -1)
				{
					printf("Error occured when opening %s, errno = %d, %s\n", inName, errno, stderror(errno));
					return -1;
				}
				outExist = 1;
				break;
		}
	}
	if(optind == argc) //No input files specified
	{
		if(readWrite() == -1) {return -1;}
		printf("Input file: %s, Total bytes written: %d, Read/Writes made: %d\n", inName, bytesWritten, countRW);
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
				if((fdIn = open(inName, O_RDONLY)) == -1)
				{
					printf("Error occured when opening %s, errno = %d, %s\n", inName, errno, stderror(errno));
					return -1;
				}

			}
			if(readWrite() == -1) {return -1;}
			if(isBinary){printf("WARNING: %s is a binary file\n", inName);}
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

