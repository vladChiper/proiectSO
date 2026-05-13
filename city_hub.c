#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


#define BUFFERSIZE 1024

int main(){
    int hub_pid;
    if((hub_pid = fork()) < 0){
        perror("Fork hub Failed\n");
        exit(-1);
    }
    else if( hub_pid == 0){
        int pfd[2];
        int pid;
        if(pipe(pfd) < 0){
            perror("Pipe Failed\n");
            exit(-1);
        }

        if( (pid = fork()) < 0 ){
            perror("Fork Failed\n");
            exit(-1);
        }

        if(pid == 0){
            close(pfd[0]);
            dup2(pfd[1], STDOUT_FILENO);
            execlp("./monitor", "monitor_reports", NULL);
            
            perror("EXECLP FAILED\n");
            exit(0);
        }
        close(pfd[1]);
        
        char buffer[BUFFERSIZE];
        int n;
        while((n = read(pfd[0], buffer, sizeof(buffer)-1)) > 0 ){
            buffer[n] = '\0';
            printf("[MONITOR] %s", buffer);
        }

        close(pfd[0]);
    }

    
    return 0;
}