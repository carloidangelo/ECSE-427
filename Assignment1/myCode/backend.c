#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "RPCbackend.h"
#include "backend.h"

#define BUFSIZE 1024

int addInts(int a, int b){
    return a + b;
}

int multiplyInts(int a, int b){
    return a*b;
}

float divideFloats(float a, float b){
    return a/b;
}

void sleepBackend(int x){
    sleep(x);
}

uint64_t factorial(int x){
    uint64_t i;
    uint64_t result = 1;
    for (i = 1; i <= x; ++i) {
        result *= i;
    }
    return result;
}

int main(int argc, char* argv[]){
    rpc_t cnct_for_client;
    char client_msg[BUFSIZE] = { 0 };
    int pid;
    int rval;
    rpc_t server_info = RPC_Init(argv[1],(uint16_t) atoi(argv[2]));
    if (server_info.error_status < 0){
        fprintf(stderr, "oh no\n");
        return -1;
    }else{
        printf("Server listening on %s:%s\n", argv[1], argv[2]);
    }
    while (1){
        cnct_for_client = RPC_Acpt(server_info);
        if (cnct_for_client.error_status < 0){
            fprintf(stderr, "oh no\n");
            return -1;
        }
        pid = fork();
        if (pid < 0){ //error occured
            fprintf(stderr, "Fork failed");
            return -1;
        }else if (pid == 0){ //child process
            break;
        }else{ //parent process
        }
    }

    while (strcmp(client_msg, "shutdown") && strcmp(client_msg, "exit")) {
        // get user request
        memset(client_msg, 0, BUFSIZE);
        RPC_Serve_Client(cnct_for_client, client_msg);
    }
    RPC_Close(cnct_for_client);
    return 0;
}
