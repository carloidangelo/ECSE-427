#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
    rpc_t server_info;
    char client_msg[BUFSIZE] = { 0 };
    int shutdown_status = 0;
    int pid;
    int rval;
    
    // create server
    server_info = RPC_Init(argv[1],(uint16_t) atoi(argv[2]));
    if (server_info.error_status < 0){
        fprintf(stderr, "oh no\n");
        return -1;
    }else{
        printf("Server listening on %s:%s\n", argv[1], argv[2]);
    }
    // accept connections from clients
    while (1){
        cnct_for_client = RPC_Acpt(server_info);
        if (cnct_for_client.error_status < 0){
            fprintf(stderr, "oh no\n");
            return -1;
        }
        waitpid(-1, &rval, WNOHANG);
        if (WEXITSTATUS(rval) != 15){
            pid = fork();
            if (pid < 0){ //error occured
                fprintf(stderr, "Fork failed");
                return -1;
            }else if (pid == 0){ //child process
                break;
            }else{ //parent process
                // continue listening for more clients
            }
        }else {
            // close server
            RPC_Close(server_info);
            printf("Server no longer listening on %s:%s\n", argv[1], argv[2]);
            shutdown_status = 1;
            break;
        }
    }

    while (strcmp(client_msg, "shutdown") && strcmp(client_msg, "exit") && !shutdown_status) {
        memset(client_msg, 0, BUFSIZE);
        // serve client
        RPC_Serve_Client(cnct_for_client, client_msg);
    }
    // close connection
    RPC_Close(cnct_for_client);

    // child returns 15 to parent after shutdown
    if (!strcmp(client_msg, "shutdown")){
        return 15;
    }
    
    // before shutdown of backend, check if there are any child processes still
    // runninng
    while(waitpid(-1, &rval,0) != -1){
    }

    return 0;
}
