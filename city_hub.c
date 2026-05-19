#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFERSIZE 1024


void cmd_start_monitor() {
    int hub_pid = fork();
    if(hub_pid < 0) {
        perror("Fork hub Failed\n");
        return;
    } else if (hub_pid == 0) { 
        int pfd[2];
        if(pipe(pfd) < 0) { perror("Pipe Failed\n"); exit(-1); } 

        int pid = fork();
        if(pid < 0) { perror("Fork Failed\n"); exit(-1); }

        if(pid == 0) { 
            // Child: rulează monitor_reports
            close(pfd[0]);
            dup2(pfd[1], STDOUT_FILENO); 
            execlp("./monitor_reports", "monitor_reports", NULL);
            perror("EXECLP FAILED\n");
            exit(1);
        }
        
        close(pfd[1]);
        char buffer[BUFFERSIZE];
        int n;
        while((n = read(pfd[0], buffer, sizeof(buffer)-1)) > 0) {
            buffer[n] = '\0';
            if (strstr(buffer, "ERR:") != NULL) {
                printf("\n[HUB_MON] Eroare la pornire monitor: %s> ", buffer + 4);
            } else if (strstr(buffer, "END:") != NULL) {
                printf("\n[HUB_MON] Monitorul a fost oprit!\n> "); 
            } else {
                printf("\n[MONITOR] %s> ", buffer);
            }
            fflush(stdout);
        }
        close(pfd[0]);
        exit(0);
    }
    printf("Monitor pornit in fundal (hub_mon PID: %d).\n", hub_pid);
}

void cmd_calculate_scores(char *args) {
    char *district = strtok(args, " \n");
    int pids[50], pfds[50][2], count = 0;
 
    while(district != NULL && count < 50) {
        if(pipe(pfds[count]) < 0) { perror("Pipe failed"); break; }
        
        pids[count] = fork();
        if(pids[count] == 0) { 
            close(pfds[count][0]);
            dup2(pfds[count][1], STDOUT_FILENO); // 
            execlp("./scorer", "scorer", district, NULL); 
            perror("Execlp scorer failed");
            exit(1);
        }
        close(pfds[count][1]);
        district = strtok(NULL, " \n");
        count++;
    }

    printf("\n--- Combined Workload Report ---\n");
    for(int i = 0; i < count; i++) {
        char buffer[BUFFERSIZE];
        int n;
        while((n = read(pfds[i][0], buffer, sizeof(buffer)-1)) > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
        }
        close(pfds[i][0]);
        waitpid(pids[i], NULL, 0); 
    }
    printf("--------------------------------\n");
}

int main() {
    char input[BUFFERSIZE];
    printf("=== City Hub Interactiv ===\n");
    printf("Comenzi disponibile: start_monitor, calculate_scores <d1> <d2>, exit\n");
    
    while(1) {
        printf("> ");
        if (!fgets(input, sizeof(input), stdin)) break;
        
        input[strcspn(input, "\n")] = 0; 
        if (strlen(input) == 0) continue;

        char *cmd = strtok(input, " ");
        char *args = strtok(NULL, ""); 

        if (strcmp(cmd, "start_monitor") == 0) {
            cmd_start_monitor();
        } else if (strcmp(cmd, "calculate_scores") == 0) {
            if (args == NULL) {
                printf("Utilizare: calculate_scores <district1> <district2> ...\n");
            } else {
                cmd_calculate_scores(args);
            }
        } else if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) {
            break;
        } else {
            printf("Comanda necunoscuta.\n");
        }
    }
    return 0;
}