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
#include <errno.h>

const int max_line_len = 20;
int whitelist_lines_allocated = 64;
int whitelist_size;
int arp_interval = 30;

FILE *fp = NULL;
FILE *output = NULL;
FILE *wl = NULL;

typedef struct Host{
    char ip[16];
    char hw_type[10];
    char flags[10];
    char mac[20];
    char iface[21];
} Host;

typedef struct HostList{
    Host* hosts;
    int len;
} HostList;

static struct option long_options[] = {
    {"whitelist",           required_argument   , NULL, 'w'},
    {"interval",            required_argument   , NULL, 'i'},
    {"daemon",              no_argument         , NULL, 'd'},
    {"logfile",             required_argument   , NULL, 'l'},
    {"quiet",               no_argument         , NULL, 'q'},
    {"help",                no_argument         , NULL, 'h'},
    {NULL, 0, NULL, 0}
};

int showHelp(){
    printf("Usage: ./arp-mon \n");
    printf("Params:\n");
    printf("\t-w, --whitelist: List of MAC addresses to be ignored by the monitor.  Default: /tmp/whitelist\n");
    printf("\t-i, --interval: Scanning interval in seconds. Default: 30\n");
    printf("\t-l, --logfile: Destination to be used as log file. Default: /tmp/monitor.log\n");
    printf("\t-d, --daemon: Run as a daemon. Should always be used with -l, or all output will be lost.\n");
    printf("\t-h, --help: Print this message.\n");
    exit(0);
}

void graceful_close(){
    if(fp) fclose(fp);
    if(output) fclose(output);
    if(wl) fclose(wl);
    exit(EXIT_SUCCESS);
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

char** read_whitelist(char *whitelist_file){
    char **whitelist = (char **) calloc(whitelist_lines_allocated,sizeof(char*));
    if(whitelist == NULL){
        fprintf(stderr, "Out of memory.\n");
        graceful_close();
    }
    file_open:
    wl = fopen(whitelist_file, "r");
    if(wl == NULL){
        printf("%d\n",errno);
        if(errno == ENOENT){  //cria whitelist se ela não existir
            fclose(fopen(whitelist_file,"w"));
            goto file_open;
        }else{        
            fprintf(stderr, "Error reading whitelist.\n");
            graceful_close();
        }

    }
    int i;
    for(i = 0; 1; ++i){
        int j;
        if(i >= whitelist_lines_allocated){
            int new_size;
            new_size = whitelist_lines_allocated * 2;
            whitelist = (char **) realloc(whitelist, sizeof(char*) * new_size);
            if(whitelist == NULL){
                fprintf(stderr, "Out of memory.\n");
                graceful_close();
            }
            whitelist_lines_allocated = new_size;
        }
        whitelist[i] = malloc(max_line_len);
        if(whitelist[i] == NULL){
            fprintf(stderr, "Out of memory.\n");
            graceful_close();
        }
        if(fgets(whitelist[i], max_line_len - 1, wl) == NULL)
            break;
        for(j = strlen(whitelist[i]) - 1; j >= 0 && (whitelist[i][j] == '\n' || whitelist[i][j] == '\r'); --j)
            whitelist[i][j] = '\0';
        
    }
    //tamanho real da lista que será usado para iterar
    whitelist_size = i+1;
    fclose(wl);
    return whitelist;
}

void signal_handler(int sig){
    switch(sig){
        case SIGTERM:
            graceful_close();
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

    //Executa o daemon com permissões rw-rw-rw-
    umask(0666);

    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--){
        close (x);
    }
}

void print_loop(char *logfile, char* whitelist_file){
    HostList h;
    char **whitelist = NULL;
    whitelist = read_whitelist(whitelist_file);
    int skip;


    while(1){
        h = read_arp();

        if(logfile) {
            output = fopen(logfile,"a+");
        }else{
            output = stdout;
        }
        
        for(int i = 0; i < h.len; i++){
            skip = 0;
            for(int j = 0; j < whitelist_size; j++){
                //se o mac estiver na whitelist, pula o print
                if(!strcmp(h.hosts[i].mac,whitelist[j])){
                    skip = 1;
                    break;
                } 
            }
            if(!skip)
                fprintf(output, "IP: %s, MAC: %s, Interface: %s\n", h.hosts[i].ip, h.hosts[i].mac,h.hosts[i].iface);

        }

        fprintf(output,"\n");
        if(logfile) fclose(output);

        sleep(arp_interval);
    }
}

int main(int argc, char** argv){
    char* logfile = "/tmp/monitor.log";
    char* whitelist_file = "/tmp/whitelist";
    int option_index;
    int opt;
    int daemonized = 0;
    int quiet = 0;
    while(1){
        option_index = 0;
        opt = getopt_long(argc, argv, "l:dq:w:hi:", long_options, &option_index);
        if(opt == -1) {
            break;
        }
        switch(opt){
            case 'i':
                arp_interval = atoi(optarg);
                break;
            case 'w':
                whitelist_file = optarg;
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
            case '?':
                showHelp();
                break;
            default:
                break;
        }                              
    }

    if(daemonized){
        daemon_skel();
        //limpa o arquivo de log existente
        fclose(fopen(logfile,"w"));
        print_loop(logfile,whitelist_file);
    }
    
    print_loop(NULL,whitelist_file);                     

    return -1;
}



