#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

FILE *logfile = NULL;
FILE *pp = NULL;
	
void graceful_close(int sig){
	fprintf(logfile,"\nClosing due to signal %s\n", strsignal(sig));
	fclose(logfile);
	exit(0);
}

int main(int argc, char **argv){
	
	struct sigaction act;

	act.sa_handler = graceful_close;
	
	sigfillset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGTERM, &act, 0);
	sigaction(SIGINT, &act, 0);
	
	logfile = fopen("./daemon.log","w");
	
	if(!logfile){
		printf("Unable to open log file. Aborting...\n");
		return 1;
	}
	if(argc != 2){
		printf("Wrong number of parameters!\n");
		return 2;
	}

	while(1){

		pp = popen("ps -eo pid,ppid,stat,comm |\
				awk 'match($3, /Z.*/) {print $1 \" \" $2 \" \" $4}'", "r");
		
		if(pp){		
			while(1){
				char *line;
				char buf[1000];
				line = fgets(buf, sizeof buf, pp);
				if (line == NULL) break;
				fprintf(logfile,"%s\n",line);
			}

			fprintf(logfile, "================\n");

			pclose(pp);
		}
			
		sleep(atoi(argv[1]));
	}

	fclose(logfile);

	return 0;
}
