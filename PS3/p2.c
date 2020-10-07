#include <fcntl.h>
#include <stdio.h>
main()
{
	int pid,fd;
	write(1,"1234",4);
	switch(pid=fork())
	{
		case -1:perror("fork");return -1;
		case 0: dup2(2,3);break;
		default: fd=open("out.txt",O_WRONLY|O_APPEND);dup2(fd,1);
			 write(1,"ABC",3);break;
	}
	/* Sketch the file descriptor tables of both parent and child processes
	 * when they have both reached this point. Also sketch the open files
	 * "table" (struct file entries) showing the connections between them
	 * and then per-process fd tables as well as to the in-core inodes.
	 * Show the values of the f_mode, f_flags, f_count and f_pos fields.
	 * It is sufficient to denote the inodes by the name of the file that
	 * they represent, similar to how these diagrams were presented in the
	 * lecture notes and in class.
	 * */
	for(;;)
		/* endless loop */;
}
