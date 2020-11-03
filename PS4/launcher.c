#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>

void checkWait(pid_t cid);
void closeAll(int fds[2]);

int main(int argc, char *argv[]){
	int fds1[2];
	int fds2[2];
	pid_t c1, c2, c3;
	char **wgArgs = malloc(sizeof(char*));
	wgArgs[0] = malloc(10*sizeof(char));
	strcpy(wgArgs[0], "./wordgen");
	char **wsArgs = malloc(2*sizeof(char*));
	wsArgs[0] = malloc(13*sizeof(char));
	wsArgs[1] = malloc(10*sizeof(char));
	strcpy(wsArgs[0], "./wordsearch");
	strcpy(wsArgs[1], "words.txt");
	char **pgArgs = malloc(sizeof(char*));
	pgArgs[0] = malloc(8*sizeof(char*));
	strcpy(pgArgs[0], "./pager");
	if(argc > 1){
		if(argc == 2){
			wgArgs = realloc(wgArgs, 2*sizeof(char*));
			wgArgs[1] = argv[argc-1];
		}
		else{
			fprintf(stderr, "Error, launcher only takes one argument: number of words to generate\n");
			return -1;
		}
	}
	if(pipe(fds1) == -1){
		perror("Failed to create pipe");
		return -1;
	}
	//have two pipes, need to create 4 redirections in total. wordgen >fds1[1] wordsearch <fds1[0] >fds2[1] pager <fds2[0]
	if(!(c1 = fork())){ 	//first child, wordgen
		dup2(fds1[1], 1);
		closeAll(fds1);
		if(execvp(wgArgs[0], wgArgs) < 0){
			perror("Error when running wordgen");
			return -1;
		}
	}
	if(pipe(fds2) == -1)
	{
		perror("Failed to create pipe");
		return -1;
	}
	if(!(c2 = fork())){		//second child, wordsearch
		dup2(fds1[0], 0);
		closeAll(fds1);
		dup2(fds2[1], 1);
		closeAll(fds2);
		if(execvp(wsArgs[0], wsArgs) < 0){
			perror("Error when running wordsearch");
			return -1;
		}
	}
	closeAll(fds1);
	if(!(c3 = fork())){		//third child, pager
		dup2(fds2[0], 0);
		closeAll(fds2);
		if(execvp(pgArgs[0], pgArgs) < 0){
			perror("Error when running pager");
			return -1;
		}
	}
	closeAll(fds2);
	checkWait(c3);
	checkWait(c2);
	checkWait(c1);
	return 0;

}

void checkWait(pid_t cid){
	pid_t wresult;
	int return_val;
	if((wresult = waitpid(cid, &return_val, 0)) == -1){
		fprintf(stderr, "Error in waiting for child %d: %s\n", cid, strerror(errno));
	}
	else{
		if(WIFSIGNALED(return_val)){
			fprintf(stderr, "Child process %d exited with signal number %d\n", wresult, WTERMSIG(return_val));
		}
		else{
			fprintf(stderr, "Child process %d exited with exit status %d\n", wresult, WEXITSTATUS(return_val));
		}
	}
}

void closeAll(int fds[2]){
	close(fds[0]);
	close(fds[1]);
}