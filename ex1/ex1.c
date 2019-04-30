#include <stdio.h>
#include <unistd.h>

extern int isopen(int fd);

int main(void){
	int nopen, fd;

	for (nopen = fd = 0; fd < getdtablesize(); fd++){
		if(isopen(fd))
			nopen++;

	}

	printf("existem %d descritores abertos\n", nopen);

	return (0);

}
	
