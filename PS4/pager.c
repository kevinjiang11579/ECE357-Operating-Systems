#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]){
	int bytesrd;
	char *lineptr = malloc(sizeof(char));
	size_t length = 0;
	int lineCount = 0;
	int lineSize = 4096;
	char *newLine = malloc(lineSize*sizeof(char));
	FILE *fstream;
	while((bytesrd = getline(&lineptr, &length, stdin))!=-1){
		fprintf(stdout, "%s", lineptr);
		lineCount++;
		if(lineCount >= 23){
			if(fstream = fopen("/dev/tty", "r+")){
				fprintf(fstream, "--- Press RETURN for more ---");
				while(*newLine != '\n'){
					fgets(newLine, lineSize, fstream);
					if(*newLine == 'q' || *newLine == 'Q'){
						if(*(newLine + 1) != '\n'){
							*newLine = 0;
						}
						else{
							return 0;
						}
					}
				}
				*newLine = 0;
				fclose(fstream);
			}
			else{
				perror("Error, unable to open terminal for reading and writing");
			}
			lineCount = 0;
		}
	}
	return 0;
}