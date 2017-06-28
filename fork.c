#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int main(void)
{

	pid_t pid;
	pid = fork();
	if (pid > 0){
	printf("parent = %d\n",pid);
	printf("getpid() = %d\n",getpid());
	printf("getppid() = %d\n",getppid());
	}else if (pid ==0){
	
	printf("child = %d\n",pid);
	}


	sleep(100);
	return 0;
}
