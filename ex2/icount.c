#include <stdio.h>
#include <unistd.h>
#include "walk_dir.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

int counter = 0;
int type_counted = S_IFREG;
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
		printf("Requested filetype found at: %s\n",path);
		counter++;	
	}

	if(recursive_mode){
		
		//tornando recursivo
		
		if((fileStat.st_mode & S_IFMT) == S_IFDIR){
			walk_dir(path,count_files);
		}	
	
	}

}

int main(int argc, char **argv){
	int opt;
	int opterr = 0;
	int total = 0;
	char *type_string = "S_IFDIR (arquivo normal)";
	while((opt = getopt(argc,argv,"Rrdlbc")) != -1){
		switch(opt){
			case 'd':
				type_counted = S_IFDIR;
				type_string = "S_IFDIR (diretorio)";
				break;
			case 'l':
				type_counted = S_IFLNK;
				type_string = "S_IFLNK (link simbolico)";
				break;	
			case 'b':
				type_counted = S_IFBLK;
				type_string = "S_IFBLK (arquivo de bloco)";
				break;
			case 'c':
				type_counted = S_IFCHR;
				type_string = "S_IFCHR (arquivo de caracter)";
				break;
			case 'R':
				recursive_mode = 1;
				break;
			case 'r':
			default:
				type_counted = S_IFREG;
				break;

		}
	}
    //printf("# args = %d\n", optind);
	for(; optind < argc; optind++){
		printf("WALKING THROUGH DIR %s\n", argv[optind]);
		walk_dir(argv[optind], count_files);
		printf("%d files of type %s on path '%s'.\n\n",counter, type_string, argv[optind]);
		total += counter;
		counter = 0;
	}
	printf("Counted %d files of type %s.\n", total, type_string);

	return 0;
}
