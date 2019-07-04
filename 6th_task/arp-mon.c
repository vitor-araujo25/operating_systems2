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
#include <getopt.h>
#include <string.h>

const char *arp = {"/proc/net/arp"};

FILE *fp = NULL;
FILE *logp = NULL;
FILE *bl = NULL;

typedef struct Host{
    char ip[16];
    char hw_type[10];
    char flags[10];
    char mac[18];
    char iface[21];
} Host;

typedef struct HostList{
    Host* hosts;
    int len;
} HostList;

static struct option long_options[] = {
    {"blacklist",           required_argument   , NULL, 'b'},
    {"daemon",              no_argument         , NULL, 'd'},
    {"logfile",             required_argument   , NULL, 'l'},
    {"quiet",               no_argument         , NULL, 'q'},
    {"help",                no_argument         , NULL, 'h'},
    {NULL, 0, NULL, 0}
};

int showHelp(){
    printf("Usage: ./sso \n");
    printf("Params:\n");
    printf("\t-b, --blacklist: Blacklist of MAC address. Alerts if detected even in quiet mode. Default: /tmp/blacklist\n");
    printf("\t-l, --logfile: Location to log events. Default: /tmp/monitor.log\n");
    printf("\t-d, --daemon: Run as a daemon\n");
    exit(0);
}

HostList read_arp(){

    Host *hosts;
    char host_count[10];
    char buf[1000];

    fp = popen("cat /proc/net/arp | tail -n +2 | wc -l","r");
    fgets(host_count,sizeof(host_count),fp);

    hosts = (Host*)malloc(sizeof(Host)*(int)atoi(host_count));
    fclose(fp);
    char *query = "cat /proc/net/arp | awk 'NR>1{printf \"%s-%s-%s-%s-%s\\n\",$1,$2,$3,$4,$6}'";

    fp = popen(query,"r");
    int i = 0;
    while (fgets(buf, sizeof(buf)-1, fp) != NULL){
        
        int ret = sscanf(buf, "%[^-\n]-%[^-\n]-%[^-\n]-%[^-\n]-%[^-\n]", (hosts+i)->ip, (hosts+i)->hw_type, (hosts+i)->flags, (hosts+i)->mac, (hosts+i)->iface);
        ++i;
    }
    fclose(fp);
    HostList list;
    list.hosts = hosts;
    list.len = (int)atoi(host_count);

    return list;
}

void signal_handler(int sig){
    switch(sig){
        case SIGTERM:
            fclose(fp);
            fclose(logp);
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

int main(int argc, char** argv){
    char* logfile = "/tmp/monitor.log";
    char* blacklist = "/tmp/blacklist";
    int option_index;
    int opt;
    int daemonized = 0;
    int quiet = 0;
    while(1){
        option_index = 0;
        opt = getopt_long(argc, argv, "l:dq:b:h", long_options, &option_index);
        if(opt == -1) {
            break;
        }
        switch(opt){
            case 'b':
                blacklist = optarg;
                break;                                                                                                                                                              
            case 'd':
                daemonized = 1;
                break;
            case 'l':
                logfile = optarg;
                break;
            case 'q':
                quiet = 1;
                break;
            case 'h':
                showHelp();
                break;
            default:
                break;
        }                              
    }
    HostList h = read_arp();
    if(daemonized){
        daemon_skel();
        while(1){
            logp = fopen(logfile,"w");
            
            for(int i = 0; i < h.len; i++){
                fprintf(logp, "IP: %s, MAC: %s, Interface: %s\n", h.hosts[i].ip, h.hosts[i].mac,h.hosts[i].iface);
            }

            fclose(logp);
            sleep(30);
        }
    }
    else{
        while(1){
            for(int i = 0; i < h.len; i++){
                printf("IP: %s, MAC: %s, Interface: %s\n", h.hosts[i].ip, h.hosts[i].mac, h.hosts[i].iface);
            }
            printf("\n");
            sleep(30);
        }
                     
    }
  
}



