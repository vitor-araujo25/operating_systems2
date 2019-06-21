#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>

FILE *fp = NULL;
FILE *pp = NULL;

void signal_handler(int sig){
    switch(sig){
        case SIGTERM:
			fprintf(fp,"\nSIGTERM recebido, encerrando daemon...");
			fclose(fp);
			exit(0);
            break;
        default:
            break;
    }
}

static void daemon_skel(){
    pid_t pid;

    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0){
        printf("PID: %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0)
        exit(EXIT_FAILURE);

    signal(SIGALRM, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR1, signal_handler);
    signal(SIGCHLD, signal_handler);

    umask(0);

    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--){
        close (x);
    }
}

int main(int argc, char **argv){
    daemon_skel();

    char *logfile = "/tmp/daemon.log";
    
    fp = fopen(logfile, "w+");
    
	fprintf(fp, "PID\t\tPPID\t\tNome do Programa\n");
	while (1){
        pp = popen("ps -eo pid,ppid,stat,comm |\
				awk 'match($3, /Z.*/) {printf \
				\"\%s\\t\\t\%s\\t\\t\%s\\n\",$1,$2,$4}'", "r");
		fprintf(fp, "==================================================\n");
        if(pp){
            while(1){
                    char *line;
                    char buf[1000];
                    line = fgets(buf, sizeof buf, pp);
                    if (line == NULL) break;
                    fprintf(fp,"%s",line);
			}
            fflush(fp);
            fclose(pp);
        }
        sleep(atoi(argv[1]));
    }

    return EXIT_SUCCESS;
}
