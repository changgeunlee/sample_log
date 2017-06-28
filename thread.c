#include <stdio.h>
#include <pthread.h>

void *makefile(void *data);

int main(void)
{
	pthread_t p_thread1;
	int status;


	pthread_create(&p_thread1, NULL, makefile, (void*)"Thread1");

	pthread_join(p_thread1, (void*)&status);	
}

void *makefile(void *data){
	printf("1");
}
