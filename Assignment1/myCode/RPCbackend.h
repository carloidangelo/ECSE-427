#ifndef RPC_BACKEND_
#define RPC_BACKEND_
#include <stdint.h>
#include "RPC.h"

rpc_t RPC_Init(char* host, uint16_t port);

rpc_t RPC_Acpt(rpc_t server_info);

void RPC_Serve_Client(rpc_t r, char* client_msg);

#endif // RPC_BACKEND_
