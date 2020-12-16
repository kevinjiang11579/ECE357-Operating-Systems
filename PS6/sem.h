#ifndef _SEM_H
#include "spinlock.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#define _SEM_H
#endif

static char *lock;
static sigset_t mask;
static void *sharedMap;
static unsigned long *mapPos;
static sigset_t USRmask;

struct taskListNode{
	pid_t taskPID;
	struct taskListNode *next;
};

struct sem{
	unsigned int count;
	unsigned long listStart;
	struct taskListNode *head;
	struct taskListNode *tail;
};
void sem_init(struct sem *s, int count){
	s->count = count;
	s->head = (struct taskListNode*)(sharedMap+*mapPos); //Give the linked list a shared address
	*mapPos += sizeof(struct taskListNode);
	s->listStart = *mapPos;
	s->head->taskPID = 0; //The tail node does not matter, only used to reset linked list
	s->head->next = NULL;
	s->tail = s->head;
}
int sem_try(struct sem *s){
	spin_lock(lock);
	if (!(s->count)){
		spin_unlock(lock);
		return 0;
	}
	else{
		s->count--;
		spin_unlock(lock);
		return 1;
	}
}
void sem_wait(struct sem *s){
	spin_lock(lock);
	while(!(s->count)){
		sigprocmask(SIG_BLOCK, &USRmask, NULL);
		//printf("sem_wait: Count is 0, adding process %d to waitlist at mapPos %ld...\n", getpid(), *mapPos);
		s->head->next = (struct taskListNode*)(sharedMap+*mapPos); //Add process to waitlist, with shared address
		*mapPos += sizeof(struct taskListNode);
		s->head = s->head->next;
		s->head->taskPID = getpid();
		s->head->next = NULL;
		//printf("Done. Address: %ld, PID:, %d, mapPos: %ld\n--------\n", (unsigned long)(s->head), s->head->taskPID, *mapPos);
		spin_unlock(lock);
		sigsuspend(&mask);
		//printf("PID %d Woke up\n--------\n", getpid());
		sigprocmask(SIG_UNBLOCK, &USRmask, NULL);
		spin_lock(lock);
	}
	s->count--;
	spin_unlock(lock);
}
void sem_inc(struct sem *s){
	spin_lock(lock);
	if(!(s->count)){
		//printf("sem_inc: Count is 0, preparing to wake sleepers and increment count\n--------\n");
		struct taskListNode *current = s->tail;
		while(current->next != NULL){ //Wake all blocked processes in the linked list
			current = current->next;
			//printf("Sending signal to PID: %d\n--------\n", current->taskPID);
			kill(current->taskPID, SIGUSR1);
		}
		s->head = s->tail; //Move back to original head of linked list
		s->head->next = NULL;
		*mapPos = s->listStart;
	}
	s->count++;
	spin_unlock(lock);
}