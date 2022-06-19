#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include<sys/wait.h>
#include <stdlib.h>



// handle client request for operation
void client_handler(){
    pid_t pid = fork();
    // fork failed
    if (0 > pid) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    // parent process return to handle clients
    if (0 < pid)
        return;
    // child process execute calculation:
    FILE* to_srv = fopen("to_srv.txt", "r");
    // file not open
    if (NULL == to_srv){
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }


    // calculate
    exit(0);
}

int main() {
    signal(SIGUSR1, client_handler);
    // server loop - waiting for clients (without busy waiting)
    while(1){
        pause();
    }
    while(wait(NULL) != -1); // wait for all child processes to finish
    //printf("Hello, World!\n");
    return 0;
}
