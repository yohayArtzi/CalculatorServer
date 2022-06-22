

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int on = 1;

// print the result by extracting info from file received by the server
void for_server_handler() {
    //printf("A - got signal from server\n");
    //alarm(0);
    on = 0;

    // get result file name
    char name[50] = "to_client_";
    char pid_str[10];
    sprintf(pid_str, "%d", (int) getpid());
    strcat(strcat(name, pid_str), ".txt");

    // open result file, read and print
    FILE *result = fopen(name, "r");
    if (NULL == result) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    //printf("B - after creating result file\n");
    char buf[50];
    fgets(buf, sizeof(buf), result);
    printf("%s\n", buf);

    // close file and delete
    if (fclose(result) != 0) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    //printf("C - after closing result file\n");
    int status = remove(name);
    if (0 != status) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    //printf("D - after removing result file\n");
}

// delete "to_srv" file if exists
void delete_to_srv() {
    //printf("DELETE: deleting to_srv file\n");
    if (0 == access("to_srv.txt", F_OK)) {
        int status = remove("to_srv.txt");
        if (0 != status) {
            printf("ERROR_FROM_EX4\n");
            exit(1);
        }
    }
}

// stop waiting for server: close file and exit
void im_tired_waiting() {
    //printf("ALARM: in alarm handler\n");
    delete_to_srv();
    printf("Client closed because no response was received from the server for 30 seconds\n");
    exit(1);
}

int main(int argc, char **argv) {
    signal(SIGUSR2, for_server_handler);
    signal(SIGALRM, im_tired_waiting);

    if (5 != argc) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }

    int i;
    for (i = 0; 0 == access("to_srv.txt", F_OK) && i < 10; i++) {
        sleep((rand() % 5) + 1);
    }
    // client is exhausted, shame on you server
    if (10 == i) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }

    //printf("1 - after checking useful things. Trying to create to_srv file\n");
    // create "to_srv.txt" file and write to it
    FILE *to_srv = fopen("to_srv.txt", "w");
    // file not open
    if (NULL == to_srv) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    char pid_str[10];
    sprintf(pid_str, "%d", (int) getpid());
    strcat(pid_str, "\n");
    fputs(pid_str, to_srv);
    for (i = 0; i < 3; i++) {
        fputs(argv[i + 2], to_srv);
        fputs("\n", to_srv);
    }
    if (fclose(to_srv) != 0) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    //printf("2 - after creating to_srv file. Trying to send signal to server\n");
    //printf("BTW pid of server is %d\n", atoi(argv[1]));
    int status = kill(atoi(argv[1]), SIGUSR1);
    if (-1 == status) {
        delete_to_srv();
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    //printf("3 - after sending signal. Trying use alarm\n");

    while (on) {
        alarm(30);
        pause();
        alarm(0);
    }
    //printf("4 - server sent signal, now I'm done\n");
    return 0;
}