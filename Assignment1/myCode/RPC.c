#include "RPC.h"
#include <unistd.h>
#include <stdio.h>

void RPC_Close(rpc_t r){
    close(r.sockfd);
}