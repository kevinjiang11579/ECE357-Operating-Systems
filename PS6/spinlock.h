#ifndef _SPINLOCK_H
#include <sched.h>
#include "tas.h"
#define _SPINLOCK_H
#endif
void spin_lock(char *lock){
	while (tas(lock)){
		sched_yield();
	}
}
void spin_unlock(char *lock){
	*lock = 0;
}