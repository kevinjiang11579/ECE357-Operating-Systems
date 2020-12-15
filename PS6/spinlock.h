#include <sched.h>
#include "tas.h"
void spin_lock(char *lock){
	while (tas(lock)){
		sched_yield();
	}
}
void spin_unlock(char *lock){
	*lock = 0;
}