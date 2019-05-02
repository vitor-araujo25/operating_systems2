#include <stdio.h>
#include <unistd.h>
#include "walk_dir.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

int counter = 0;
int type_counted;
int recursive_mode = 0;

void count_files(char *path){
	
	struct stat fileStat;
	
	//man stat(2)
	if(lstat(path, &fileStat) == -1){
		printf("Error: %s", strerror(errno));
		return -1;
	}
	
	//comportamento definido em inode(7)
	if((fileStat.st_mode & S_IFMT) == type_counted){
		printf("OIE!!\n");
		counter++;	
	}

	if(recursive_mode){
		
		//tornando recursivo(não funciona)
		
		if((fileStat.st_mode & S_IFMT) == S_IFDIR){
			walk_dir(path,count_files);
		}	
	
	}

}

int main(int argc, char **argv){
	int opt;
	int opterr = 0;
	while((opt = getopt(argc,argv,"Rrdlbc")) != -1){
		switch(opt){
			case 'd':
				type_counted = S_IFDIR;
				break;
			case 'l':
				type_counted = S_IFLNK;
				break;	
			case 'b':
				type_counted = S_IFBLK;
				break;
			case 'c':
				type_counted = S_IFCHR;
				break;
			case 'R':
				recursive_mode = 1;
				break;
			case '?':
			case 'r':
			default:
				type_counted = S_IFREG;
				break;

		}
	}
	printf("type_counted: %d,  S_IFREG: %d\n",type_counted,S_IFREG);
    printf("# args = %d\n", optind);
	for(; optind < argc; optind++){
		printf("WALKING THROUGH DIR %s\n", argv[optind]);
		walk_dir(argv[optind], count_files);
		// walk_dir("/home", count_files);
	}

	printf("TOTAL: %d arquivos do tipo pedido.\n",counter);
	return 0;
}
