#include <stdio.h>
#include <unistd.h>
#include "walk_dir.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

int counter = 0;
int type_counted = S_IFREG;
int recursive_mode = 0;
int opterr = 0;

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

void printHelp();

int main(int argc, char **argv){
	int opt;
	int total = 0;
	char *type_string = "S_IFDIR (arquivo normal)";
	while((opt = getopt(argc,argv,"Rrdlbch")) != -1){
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
			case 'h':
				printHelp();
				break;
			case '?':
				if(isprint(optopt)){
					fprintf(stderr,"-%c is not a valid option.\n\n", optopt);
				}else{
					fprintf(stderr,"Unknown character \\x%x \n\n",optopt);
				}
				printHelp();
				break;
			case 'r':
			default:
				type_counted = S_IFREG;
				break;

		}
	}
	if(optind == 1){
		argv[optind] = ".";
		argc++;
	}
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


void printHelp(){
	fprintf(stderr, "\
		\rInstructions:\n\
		\r\ticount -[Rrdcbl] [<dirpath>...]\n\n\
		\r\t-h         Prints these instructions and exits.\n\n\
		\rOPERATION MODES\n\
		\r\t-R  	   Runs in recursive mode.\n\n\
		\rSEARCH TYPES\n\
		\r\t-r         regular files (same as no flag)\n\
		\r\t-d         directories\n\
		\r\t-c         character files\n\
		\r\t-b         block files\n\
		\r\t-l         symbolic links\n\
		\r\n");

	exit(-1);
}
