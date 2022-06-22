

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include<sys/wait.h>
#include <stdlib.h>
#include <string.h>

// maximum amount of digits for a number represented by int
#define MAX 20
#define PLUS 1
#define MINUS 2
#define MULT 3
#define DIV 4

int calculate(int operation, int num1, int num2);

void client_handler();

void time_over();


int main() {
    signal(SIGUSR1, client_handler);
    signal(SIGALRM, time_over);

    // delete "to_srv" file if exists
    if (0 == access("to_srv.txt", F_OK)) {
        int status = remove("to_srv.txt");
        if (0 != status) {
            printf("ERROR_FROM_EX4\n");
            exit(1);
        }
    }

    // run server - waiting for clients (without busy waiting)
    while (1) {
        alarm(60);
        pause();
        //alarm(0);
    }

}

// stop server and exit
void time_over() {
    while (wait(NULL) != (-1));
    printf("The server was closed because no service request was received for the last 60 seconds\n");
    exit(0);
}

// handle client request for operation
void client_handler() {
    pid_t pid = fork();
    // fork failed
    if (0 > pid) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    // parent process return to handle clients
    if (0 < pid)
        return;
    // in child process. execute calculation:
    FILE *to_srv = fopen("to_srv.txt", "r");
    // file not open
    if (NULL == to_srv) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    char buf[MAX];
    int nums[2], operation, client_pid, i = 0;
    for (; fgets(buf, MAX, to_srv); i++) {
        if (0 == i) {
            client_pid = atoi(buf);
            //printf("client pid is %d", client_pid);
        } else if (1 == i) {
            nums[0] = atoi(buf);
        } else if (2 == i) {
            operation = atoi(buf);
        } else {
            nums[1] = atoi(buf);
        }
    }
    if (fclose(to_srv) != 0) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    if (remove("to_srv.txt") != 0) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    // create client file and write the result there
    char name[50] = "to_client_";
    char pid_str[30];
    sprintf(pid_str, "%d", client_pid);
    strcat(strcat(name, pid_str), ".txt");
    FILE *to_client = fopen(name, "w");
    // file not open
    if (NULL == to_client) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    // if calculation requires division by zero
    if (DIV == operation && 0 == nums[1]) {
        fputs("CANNOT_DIVIDE_BY_ZERO", to_client);
    } else {
        int result = calculate(operation, nums[0], nums[1]);
        char result_str[MAX];
        if (0 == result)
            fputs("0", to_client);
        else {
            sprintf(result_str, "%d", result);
            fputs(result_str, to_client);
        }
    }
    if (fclose(to_client) != 0) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    int kill_val = kill(client_pid, SIGUSR2);
    if (-1 == kill_val) {
        if (remove(name) != 0) {
            printf("ERROR_FROM_EX4\n");
            exit(1);
        }
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    exit(0); // delete child process
}

// calculate client request
int calculate(int operation, int num1, int num2) {
    //printf("exercise is: %d %d %d\n", num1, operation, num2);
    if (PLUS == operation)
        return num1 + num2;
    else if (MINUS == operation)
        return num1 - num2;
    else if (MULT == operation)
        return num1 * num2;
    float result = num1/num2;
    return result;
}