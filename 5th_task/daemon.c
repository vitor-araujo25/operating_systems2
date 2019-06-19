#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>

FILE *fp = NULL;
FILE *pp = NULL;

void graceful_close(int sig){
	fprintf(fp,"\nClosing due to signal %s\n", strsignal(sig));
	fclose(fp);
	exit(0);
}

void signal_handler(int sig){
    switch(sig){
        case SIGTERM:
            graceful_close(sig);
            break;
        default:
            break;
    }
}

static void daemon_skel(){
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0){
        printf("PID: %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    /* On success: The child process becomes session leader */
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

    // /* Fork off for the second time*/
    // pid = fork();

    // /* An error occurred */
    // if (pid < 0)
    //     exit(EXIT_FAILURE);

    // /* Success: Let the parent terminate */
    // if (pid > 0){
    //     printf("PID: %d\n", pid);
    //     exit(EXIT_SUCCESS);
    // }

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    // chdir("/");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--){
        close (x);
    }

    /* Open the log file */
    // openlog ("firstdaemon", LOG_PID, LOG_DAEMON);
}

int main(int argc, char **argv){
    daemon_skel();

    char *logfile = "/tmp/daemon.log";
    // DIR *proc = opendir("/proc");
    // struct dirent* ent;
    // pid_t tgid;

    // if(proc == NULL){
    //     return 1;
    // }

    fp = fopen(logfile, "w+");
    // syslog (LOG_NOTICE, "First daemon started.");
    while (1){
        // while(ent = readdir(proc)){
        //     if(!isdigit(*ent->d_name))
        //         continue;
        //     tgid = strtol(ent->d_name, NULL, 10);
        //     fprintf (fp, "PROC PID: %d.", tgid);
        // }
        pp = popen("ps -eo pid,ppid,stat,comm |\
				awk 'match($3, /Z.*/) {print $1 \" \" $2 \" \" $4}'", "r");
        fprintf(fp, "PID\tPPID\tname\n");
        if(pp){
            while(1){
                    char *line;
                    char buf[1000];
                    line = fgets(buf, sizeof buf, pp);
                    if (line == NULL) break;
                    fprintf(fp,"%s\n",line);
                }
            fprintf(fp, "==================================================\n");
            fflush(fp);
            fclose(pp);
        }
        sleep(atoi(argv[1]));
    }

    // syslog (LOG_NOTICE, "First daemon terminated.");
    // closelog();

    return EXIT_SUCCESS;
}