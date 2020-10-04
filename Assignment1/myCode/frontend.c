#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "RPCfrontend.h"

#define BUFSIZE 1024

int main(int argc, char* argv[]){
    char user_input[BUFSIZE] = { 0 };
    char server_msg[BUFSIZE] = { 0 };
    char* token;
    rpc_t socket_info = RPC_Connect(argv[1],(uint16_t) atoi(argv[2]));
    if (socket_info.error_status < 0){
        fprintf(stderr, "oh no\n");
        return -1;
    }
    while (strcmp(user_input, "shutdown") && strcmp(user_input, "exit")) {
        memset(user_input, 0, sizeof(user_input));
        memset(server_msg, 0, sizeof(server_msg));
        // prompt user
        printf(">> ");
        // read user input from command line
        if (fgets(user_input, BUFSIZE, stdin) != NULL){
            // send input to RPC
            RPC_Call(socket_info, user_input);
        }
        RPC_getAns(socket_info, server_msg);
        printf("%s\n", server_msg);

        token = strtok(user_input, "\n");
        token = strtok(user_input, " ");
        sprintf(user_input,"%s", token);
    }
    RPC_Close(socket_info);
    return 0;
}
