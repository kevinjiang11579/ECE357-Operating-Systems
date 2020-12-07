#include "spinlock.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

void increment10B(unsigned long *amount);

int main(int argc, char *argv[]){
	void *mmapAddr;
	long int memSize = 4096;
	unsigned long *amount;
	char *lock;
	if((mmapAddr = mmap(NULL, memSize, PROT_WRITE|PROT_READ, MAP_ANONYMOUS|MAP_SHARED, -1, 0)) == MAP_FAILED){
		perror("Error in mapping shared anonymous memory with read/write protection in shared mode");
		return -1;
	}
	else{
		//Do some modification in mmap
		amount = (unsigned long*)mmapAddr;
		*amount = 0;
		lock = (char*)(mmapAddr+sizeof(*amount));
		*lock = 0;
		for(int j = 0; j < 4; j++){
			if(!fork()){
				spin_lock(lock);
				increment10B(amount);
				spin_unlock(lock);
				return 0;
			}
		}
		while(wait(NULL) != -1){};
		fprintf(stdout, "Final value of amount: %ld\n", *amount);
		munmap(mmapAddr, memSize);
	return 0;
	}
}

void increment10B(unsigned long *amount){
	for(int i = 0; i < 10000000; i++){
		(*amount)++;
	}
}