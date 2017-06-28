#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define MMAP_FILENAME "test_mmap"

int main(int argc, char **argv)
{
	int fd;
	char *file = '\0'; 
	struct stat sb;
	int flag= PROT_WRITE|PROT_READ;
	int i;

	fd = open(argv[1],O_RDWR|O_CREAT,0664);
	fstat(fd,&sb);
	printf("%d\n",sb.st_uid);
	file = (char*)malloc(40);
	if (( file = (char*)mmap(0,40,flag,MAP_SHARED,fd,0))== -1)
	{
		perror("mmap error");
		exit(1);
	}
	for(i=0; i<40; i++){
		printf("file[%d]= %d\n", i, file[i]);
	}
	memset(file, 0xff,40);
	for(i=0; i<40; i++){
		printf("file[%d]= %d\n", i, file[i]);
	}
	free(file);
	close(fd);
	munmap(file,40);
}
