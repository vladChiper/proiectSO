#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>

volatile sig_atomic_t keep_running = 1;

// Handler pentru SIGINT 
void handle_sigint(int sig) {
    keep_running = 0;
}

// Handler pentru SIGUSR1
void handle_sigusr1(int sig) {
    const char *msg = "Monitor: Un nou raport a fost adaugat.\n";
    write(STDOUT_FILENO, msg, strlen(msg));
}

int main() {

    FILE *f = fopen(".monitor_pid","r");
    if( f != NULL){
        int existing_pid;
        if(fscanf(f, "%d", &existing_pid) == 1){
            if(kill(existing_pid, 0) == 0){
                char err_msg[64];
                int len;
                len = sprintf(err_msg, "Monitorul era deja pornit cu pid: %d\n", existing_pid);
                write(STDOUT_FILENO, err_msg, len);
                fclose(f);
                return 1;
            }
        }
    }
    fclose(f);


    struct sigaction sa_int, sa_usr1;

    // Configurare handler pentru SIGINT si SIGUSR1
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_usr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    //creaza fisierul .monitor_pid cu PID-ul curent
    pid_t my_pid = getpid();
    int fd = open(".monitor_pid", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Eroare la crearea .monitor_pid");
        return 1;
    }

    char pid_str[32];
    int len = sprintf(pid_str, "%d\n", my_pid);
    write(fd, pid_str, len);
    close(fd);

    printf("Monitor pornit cu PID %d.\n", my_pid);

    // Asteapta semnale, tine programul in viata pana la SIGINT
    while (keep_running) {
        pause();
    }

    //curatare la iesire
    const char *exit_msg = "\nMonitor oprit (SIGINT). Curatare...\n";
    write(STDOUT_FILENO, exit_msg, strlen(exit_msg));
    unlink(".monitor_pid"); 

    return 0;
}