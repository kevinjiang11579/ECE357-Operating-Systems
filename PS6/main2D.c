//Part 2D, acid test. Single writer and reader has already been tested//
#include "fifo.h"
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>

void handler(int sn){
	//printf("SIGUSR1 recieved by PID: %d\n--------\n", getpid());
}

int main(int argc, char *argv[]){
	struct fifo *f;
	sharedMap = mmap(NULL, 4096, PROT_WRITE|PROT_READ, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	mapPos = (unsigned long*)sharedMap;
	*mapPos = sizeof(unsigned long);
	lock = (char*)(sharedMap + *mapPos); //lock is the atomicity spinlock for all sems
	*mapPos += sizeof(char);
	*lock = 0;
	f = (struct fifo*)(sharedMap + *mapPos);
	*mapPos += sizeof(struct fifo);
	fifo_init(f);
	printf("Done Initialzing FIFO\n");
	sigfillset(&mask);
	sigdelset(&mask, SIGUSR1);
	sigemptyset(&USRmask);
	sigaddset(&USRmask, SIGUSR1);
	signal(SIGUSR1, handler);
	unsigned long receivedNumber;
	unsigned long sentNumber;
	unsigned long allNumbers[2900];
	unsigned long sortedNumbers[29][100];
	char error = 0;
	for(int k = 0; k<4000; k++){
		printf("Test iteration %d:\n", k);
		for(int j = 0; j < 30; j++){
			if(!fork()){
				if(j<29){
					for(unsigned long i = 0; i<100; i++){
						sentNumber = j*100 + i;
						//printf("Sending number: %ld\n", sentNumber);
						fifo_wr(f, sentNumber);
					}
					return 0;
				}
				else{
					for(int i = 0; i<2900; i++){
						allNumbers[i] = fifo_rd(f);
						//printf("Received number: %ld\n", receivedNumber);
					}
					for(int i = 0; i<2899; i++){
						for(int j = i+1; j<2900; j++){
							if(allNumbers[i] == allNumbers[j])
								printf("There is a duplicate at indices %d and %d: %ld\n", i, j,allNumbers[i]);
								error = 1;
						}
					}
					for(int i = 0; i< 2900; i++){
						sortedNumbers[allNumbers[i]/100][allNumbers[i]%100] = allNumbers[i];
					}
					for(int i = 0; i<29; i++){
						for(int j = 0; j<99; j++){
							for(int k = j+1; k<100; k++){
								if(sortedNumbers[i][j] > sortedNumbers[i][k])
									printf("Fork %d is out of order\n", i);
									error = 1;
							}
						}
					}
					printf("All numbers have been recieved. If there are no extra messages, then there were no errors\n");
					//printf("Last number: %ld\n", receivedNumber);
					return 0;
				}
			}
		}
		int pcount;
		pcount = 0;
		while(wait(NULL) != -1){
			pcount++;
		}
		printf("All %d forks returned\n", pcount);
	}
	if(error){
		printf("There were some errors, see messages\n");
	}
	else{
		printf("Everything OK\n");
	}
	return 0;
}