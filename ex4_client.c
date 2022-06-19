#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

// call server for calculation and print the result
void for_server_handler(){

}

int main(int argc, char** argv) {
    signal(SIGUSR2, for_server_handler);

    if(5 != argc){
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }

    FILE *to_client = fopen("to_srv.txt", "w");
    // file not open
    if (NULL == to_client) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
}