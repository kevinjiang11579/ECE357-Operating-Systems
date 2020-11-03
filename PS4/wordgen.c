#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]){
	long numWords = -1;
	int maxLength = 15, minLength = 3;
	int maxASCII = 90, minASCII = 65;
	char **endptr = malloc(sizeof(char**));
	char *currentWord = malloc(sizeof(char));
	if(argc > 1){
		if(argc == 2){
			errno = 0;
			if(!(numWords = strtol(argv[argc-1], endptr, 10)))
				numWords = -1;
		}
		else{
			fprintf(stderr, "Error: wordgen only takes one argument, which is number of random words to generate\n");
			return -1;
		}
	}
	while(numWords != 0){
		int wordLength = rand()%(maxLength - minLength + 1) + minLength;
		currentWord = realloc(currentWord, wordLength*sizeof(char)+1);
		for(int i = 0; i < wordLength; i++){
			currentWord[i] = rand()%(maxASCII - minASCII + 1) + minASCII;
		}
		currentWord[wordLength] = 0;
		fprintf(stdout, "%s\n", currentWord);
		numWords--;
	}
	return 0;
}