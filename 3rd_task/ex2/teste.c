#include <stdio.h>
//#include <dirent.h>
#include "walk_dir.h"

int main(int argc, char **argv){
	struct dirent *d;
	DIR *dirp;
	if((dirp = opendir(argv[1])) == NULL)
		return -1;
	while((d = readdir(dirp)) != NULL){
		printf("%s, %d\n",d->d_name,d->d_type);
	}
	return 0;
}
