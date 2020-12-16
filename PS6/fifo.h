#ifndef _FIFO_H
#include "sem.h"
#include <sys/mman.h>
#include <stdio.h>
#define _FIFO_H
#endif
#define MYFIFO_BUFSIZ 4096

struct sem *writespin; //semaphore as a spinlock for fifo write, to be initialized with count 1
struct sem *readspin;

struct fifo{
	unsigned long *fptr;
	int wrPos;
	int rdPos;
	struct sem *wr;
	struct sem *rd;
};
void fifo_init(struct fifo *f){
	printf("Initializing FIFO\n");
	f->wrPos = MYFIFO_BUFSIZ/sizeof(unsigned long) - 1;
	f->rdPos = MYFIFO_BUFSIZ/sizeof(unsigned long) - 1;
	f->fptr = (unsigned long*)mmap(NULL, MYFIFO_BUFSIZ, PROT_WRITE|PROT_READ, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	f->wr = (struct sem*)(sharedMap+*mapPos); //Give s a shared address
	*mapPos += sizeof(struct sem);
	f->rd = (struct sem*)(sharedMap+*mapPos); //Give s a shared address
	*mapPos += sizeof(struct sem);
	writespin = (struct sem*)(sharedMap+*mapPos); //Give s a shared address
	*mapPos += sizeof(struct sem);
	readspin = (struct sem*)(sharedMap+*mapPos); //Give s a shared address
	*mapPos += sizeof(struct sem);
	sem_init(f->wr, MYFIFO_BUFSIZ/sizeof(unsigned long));
	sem_init(f->rd, 0);
	sem_init(writespin, 1);
	sem_init(readspin,1);

}
void fifo_wr(struct fifo *f, unsigned long d){ //Count of sem wr is how much space is left in FIFO
	while(!sem_try(writespin)){  //try spinlock, if spinlock count already 0 then loop, this is atomic
		sched_yield();
	}
	//printf("fifo_wr: wr count: %d\n", f->wr->count);
	sem_wait(f->wr);
	*(f->fptr + f->wrPos) = d;
	if(f->wrPos){
			f->wrPos = f->wrPos - 1;
	}
	else{
		f->wrPos = MYFIFO_BUFSIZ/sizeof(unsigned long) - 1;
	}
	sem_inc(f->rd); //new data in fifo, increment read semaphore count, may wake up some processes
	//printf("fifo_wr: rd count: %d\n", f->rd->count);
	sem_inc(writespin); //unlock spinlock
}
unsigned long fifo_rd(struct fifo *f){ //Count of sem rd is how much items left in FIFO
	while(!sem_try(readspin)){  //try spinlock, if spinlock count already 0 then loop, this is atomic
		sched_yield();
	}
	//printf("fifo_rd: rd count: %d\n", f->rd->count);
	sem_wait(f->rd);
	unsigned long returnVal;
	returnVal = *(f->fptr + f->rdPos);
	if(f->rdPos){
			f->rdPos = f->rdPos - 1;
	}
	else{
		f->rdPos = MYFIFO_BUFSIZ/sizeof(unsigned long) - 1;
	}
	sem_inc(f->wr); //new data in fifo, increment write semaphore count, may wake up some processes
	//printf("fifo_rd: wr count: %d\n", f->wr->count);
	sem_inc(readspin); //unlock spinlock
	return returnVal;
}