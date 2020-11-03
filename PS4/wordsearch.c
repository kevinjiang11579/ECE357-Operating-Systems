#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>

void pipeHandler(int sn);
jmp_buf int_jb;

int main(int argc, char *argv[]){
	long matchCount = 0;
	long wordCount = 0;
	FILE *fstream;
	size_t length = 1;
	int bytesrd;
	char *lineptr = malloc(sizeof(char));
	char **dictionary = malloc(sizeof(char*));
	int lineSize = 4096;
	char *newLine = malloc(lineSize*sizeof(char));
	sigset_t set;
	sigemptyset(&set);
	struct sigaction newAct = {.sa_handler = pipeHandler, .sa_mask = set, .sa_flags = 0};
	sigaction(SIGPIPE, &newAct, NULL);
	if(argc > 1){
		if(argc == 2){
			char *fileName = argv[argc-1];
			if(!(fstream = fopen(fileName, "r"))){
				perror("Error, unable to open file for reading");
				return -1;
			}
			while((bytesrd = getline(&lineptr, &length, fstream))!=-1){
				dictionary[wordCount] = malloc(sizeof(char));
				int strIndex = 0;
				while(*(lineptr + strIndex)){
					*(dictionary[wordCount] + strIndex) = toupper(*(lineptr + strIndex));
					strIndex++;
					dictionary[wordCount] = realloc(dictionary[wordCount], (strIndex+1)*sizeof(char));
				}
				*(dictionary[wordCount] + strIndex) = 0;
				wordCount++;
				dictionary = realloc(dictionary, (wordCount+1)*sizeof(char*));
			}
		}
		else{
			fprintf(stderr, "Error: wordsearch only takes one argument, which is dictionary file\n");
			return -1;
		}
	}
	if(!setjmp(int_jb))
	{
		while(fgets(newLine, lineSize, stdin)){
			for(int i = 0; i < wordCount; i++){
				if(strcmp(newLine, dictionary[i]) == 0){
					fprintf(stdout, "%s", newLine);
					matchCount++;
				}
			}

		}
	}
	fprintf(stderr, "Number of matched words: %ld\n", matchCount);
	return 0;
}

void pipeHandler(int sn){
	fprintf(stderr, "Process %d received signal %d\n", getpid(), sn);
	longjmp(int_jb, 1);
}
