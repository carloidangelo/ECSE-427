#ifndef RPC_
#define RPC_

// define a type that contains socket info and error info
typedef struct{
 int sockfd;
 int error_status; // -1 if error occurs, 0 otherwise
}rpc_t;

void RPC_Close(rpc_t r);

#endif // RPC_
