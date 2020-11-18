#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	int fd;
	void *mmapAddr;
	struct stat stbuf;
	long int fileSize = 0;
	int pageSize = getpagesize();
	char *pageBuf = malloc((pageSize+1)*sizeof(char));
	char* currentString = malloc((pageSize+1)*sizeof(char));
	int argvIndex = 0;
	while(*(argv[1]+argvIndex) != 0){ //Get length of target string
		argvIndex++;
	}
	for(int fileIndex  = 3; fileIndex < argc; fileIndex++)
	{
		if((fd = open(argv[fileIndex], O_RDWR)) == -1){
			perror("Error opening file for read and write");
			continue;
		}
		else
		{
			if(fstat(fd, &stbuf) == -1){
				perror("Error getting stats of file");
				continue;
			}
			else{
				fileSize = stbuf.st_size;
			}
			if((mmapAddr = mmap(NULL, fileSize, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED){
				perror("Error in mapping file with read/write protection in shared mode");
				return -1;
			}
			else{
				//Search for target string argv[1] and replace with string argv[2]
							close(fd);
				int pageNum = fileSize/pageSize;
				for(int i = 0; i <= pageNum; i++){
					char *mmapOffset = mmapAddr+i*pageSize;
					strncpy(pageBuf, mmapOffset, pageSize);
					for(int startChar = 0; startChar < pageSize - argvIndex; startChar++){
						if(isspace(pageBuf[startChar])){continue;}
						for(int charCount = startChar; charCount < startChar + argvIndex; charCount++){
							int zeroedIndex = charCount - startChar;
							currentString[zeroedIndex] = pageBuf[charCount];
							currentString[zeroedIndex+1] = 0;
							if(!strcmp(argv[1], currentString)){ //Target found
								for(int j = 0; j < argvIndex; j++){ //Replace string in mmap
									*(mmapOffset+startChar+j) = *(argv[2]+j);
								}
							}
						}
					}
				}
				if(msync(mmapAddr, fileSize, MS_SYNC) == -1){
					perror("Error in msync");
					return -1;
				}
				munmap(mmapAddr, fileSize);
			}
		}
	}
	return 0;
}