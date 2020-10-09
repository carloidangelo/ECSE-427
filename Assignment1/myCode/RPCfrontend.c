/*
CLIENT STUB
*/
#include "a1_lib.h"
#include "RPCfrontend.h"

#define BUFSIZE 1024

rpc_t RPC_Connect(char* ip, uint16_t port){
    rpc_t current_client = {0,0};
    if (connect_to_server(ip, port, &(current_client.sockfd)) < 0){
        current_client.error_status = -1;
    }else{
        current_client.error_status = 0;
    }
    return current_client;
}

void RPC_Call(rpc_t r, char* input){
    send_message(r.sockfd, input, BUFSIZE);
}

void RPC_getAns(rpc_t r, char* server_msg){
    ssize_t byte_count = recv_message(r.sockfd, server_msg, BUFSIZE);
}

int RPC_check_cnt(rpc_t r){
    struct sockaddr addr;
    socklen_t len;
    int status = getpeername(r.sockfd, &addr, &len);
    return status;
}
