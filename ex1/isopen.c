#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

int isopen(int fd){
	int return_value = fcntl(fd, F_GETFL);
	int err = errno;
	//printf("fd: %d, retorno: %d, errno: %d\n", fd, return_value, err);
	return return_value != -1 || err != EBADF;

}
