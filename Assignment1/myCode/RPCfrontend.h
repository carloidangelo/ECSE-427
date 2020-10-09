#ifndef RPC_FRONTEND_
#define RPC_FRONTEND_
#include <stdint.h>
#include "RPC.h"

rpc_t RPC_Connect(char* ip, uint16_t port);

void RPC_Call(rpc_t r, char* input);

void RPC_getAns(rpc_t r, char* server_msg);

int RPC_check_cnt(rpc_t r);

#endif // RPC_FRONTEND_
