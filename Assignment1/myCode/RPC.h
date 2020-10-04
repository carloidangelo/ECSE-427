#ifndef RPC_
#define RPC_

typedef struct{
 int sockfd;
 int error_status; // -1 if error occurs, 0 otherwise
}rpc_t;

#endif // RPC_
