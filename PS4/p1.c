#include <sys/signal.h>
#include <stdio.h>
#include <setjmp.h>
jmp_buf wormhole;
void handler(int sig)
{
	static int i;++i;fprintf(stderr,"In handler instance %d\n",i);longjmp(wormhole,i);
}
main()
{
	/* The keyword volatile prevents the compiler from optimizing away
	 * *the accesses to *p*/
	volatile char *p=0;
	signal(SIGSEGV,handler);
	if (setjmp(wormhole))
	{
		fprintf(stderr,"The other side\n");
		*p++;
	}
	*p--;
	return 1;
}
