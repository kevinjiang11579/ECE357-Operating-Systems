//Part 2B, test for sem.h
#include "sem.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/wait.h>

void handler(int sn){
	//printf("SIGUSR1 recieved by PID: %d\n--------\n", getpid());
}

int main(int argc, char *argv[]){
	sharedMap = mmap(NULL, 8192, PROT_WRITE|PROT_READ, MAP_ANONYMOUS|MAP_SHARED, -1, 0); //create shared mapping
	mapPos = (unsigned long*)sharedMap; //mapPos is stored in share mapping so all process see same value
	*mapPos = sizeof(unsigned long); //mapPos indicates start of unused memory in memory mapping, mapPos itself already took space
	lock = (char*)(sharedMap+*mapPos); //Give lock a shared address
	*mapPos += sizeof(char); //Increment mapPos to new start of unused memory
	struct sem *s;
	s = (struct sem*)(sharedMap+*mapPos); //Give s a shared address
	*mapPos += sizeof(struct sem);
	sigfillset(&mask);
	sigdelset(&mask, SIGUSR1);
	sigemptyset(&USRmask);
	sigaddset(&USRmask, SIGUSR1);
	signal(SIGUSR1, handler);
	sem_init(s, 5);
	for(int k = 0; k<1000; k++){
		for(int j = 0; j < 100; j++){
			if(!fork()){
				if(!(j%2)){
					for(int i = 0; i<100; i++){
						sem_wait(s);
					}
					return 0;
				}
				else{
					for(int i = 0; i<100; i++){
						sem_inc(s);
					}
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
		printf("Initial count: 5, Final count: %d\n", s->count);
	}
	printf("Everything OK\n");
	return 0;
}