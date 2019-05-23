#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

int isopen(int fd){
	int return_value = fcntl(fd, F_GETFL);
	int err = errno;
	//printf("fd: %d, retorno: %d, errno: %d\n", fd, return_value, err);
	if(return_value != -1)
		if((return_value & O_ACCMODE) == O_RDONLY)
			printf("fd %d está aberto em modo Read-Only\n",fd);
		if((return_value & O_ACCMODE) == O_RDWR)
			printf("fd %d está aberto em modo Read/Write\n",fd);
		if((return_value & O_ACCMODE) == O_WRONLY)
			printf("fd %d está aberto em modo Write-Only\n",fd);
			
	return return_value != -1 || err != EBADF;

}
