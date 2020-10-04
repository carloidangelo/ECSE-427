#include "backend.h"
#include "a1_lib.h"
#include "RPCbackend.h"
#define BUFSIZE 1024
#define MAXLENGTH 15

rpc_t RPC_Init(char* host, uint16_t port){
    rpc_t current_server = {0,0};
    if (create_server(host, port, &(current_server.sockfd)) < 0){
        current_server.error_status = -1;
    }else{
        current_server.error_status = 0;
    }
    return current_server;
}

rpc_t RPC_Acpt(rpc_t server_info){
    rpc_t cnct_for_client = {0,0};
    if (accept_connection(server_info.sockfd, &(cnct_for_client.sockfd)) < 0){
        cnct_for_client.error_status = -1;
    }else{
        cnct_for_client.error_status = 0;
    }
    return cnct_for_client;
}

void RPC_Serve_Client(rpc_t r, char* client_msg){
    char* token;
    char tokens[MAXLENGTH][MAXLENGTH] = { 0 };
    int count;
    char result_str[BUFSIZE] = { 0 };

    memset(tokens, 0, sizeof(tokens));
    memset(result_str, 0, sizeof(result_str));

    ssize_t byte_count = recv_message(r.sockfd, client_msg, BUFSIZE);
    token = strtok(client_msg, "\n");
    token = strtok(client_msg, " ");
    count = 0;
    while(token != NULL) {
        strcpy(tokens[count], token);
        token = strtok(NULL, " ");
        count++;
    }
    if (strcmp(tokens[0], "add") == 0){
        sprintf(result_str, "%d", addInts(atoi(tokens[1]),atoi(tokens[2])));
    }else if (strcmp(tokens[0], "multiply") == 0){
        sprintf(result_str, "%d", multiplyInts(atoi(tokens[1]),atoi(tokens[2])));
    }else if (strcmp(tokens[0], "divide") == 0){
        if (atof(tokens[2]) == 0.0){
            sprintf(result_str, "%s", "Error: Division by zero");
        }else{
            sprintf(result_str, "%f", divideFloats(atof(tokens[1]),atof(tokens[2])));
        }
    }else if (strcmp(tokens[0], "factorial") == 0){
        sprintf(result_str, "%lu", factorial(atoi(tokens[1])));
    }else if (strcmp(tokens[0], "sleep") == 0){
        sleep(atoi(tokens[1]));
    }else if (strcmp(tokens[0], "shutdown") == 0){
        sprintf(client_msg,"%s","shutdown");
        sprintf(result_str,"%s","Bye!");
    }else if (strcmp(tokens[0], "exit") == 0){
        sprintf(client_msg,"%s"," ");
        sprintf(result_str,"%s","Bye!");
    }else{
        char error_msg[BUFSIZE]  = "Error: Command {";
        strcat(error_msg, tokens[0]);
        strcat(error_msg, "} not found");
        sprintf(result_str,"%s", error_msg);
    }
    send_message(r.sockfd, result_str, BUFSIZE);
}