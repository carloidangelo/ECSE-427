#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

void sleep(int x){
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
    char client_msg[BUFSIZE] = { 0 };
    rpc_t server_info = RPC_Init(argv[1],(uint16_t) atoi(argv[2]));
    if (server_info.error_status < 0){
        fprintf(stderr, "oh no\n");
        return -1;
    }else{
        printf("Server listening on %s:%s\n", argv[1], argv[2]);
    }

    rpc_t cnct_for_client = RPC_Acpt(server_info);
    if (cnct_for_client.error_status < 0){
        fprintf(stderr, "oh no\n");
        return -1;
    }

    while (strcmp(client_msg, "shutdown")) {
        // get user request
        memset(client_msg, 0, BUFSIZE);
        RPC_Serve_Client(cnct_for_client, client_msg);
    }
    return 0;
}
