#include <stdio.h>
#include <string.h>

#include "a1_lib.h"

#define BUFSIZE   128

int main(void) {
  int sockfd, clientfd;
  char msg[BUFSIZE];
  const char *greeting = "hello, world";
  int running = 1;

  if (create_server("0.0.0.0", 15150, &sockfd) < 0) {
    fprintf(stderr, "oh no\n");
    return -1;
  }

  if (accept_connection(sockfd, &clientfd) < 0) {
    fprintf(stderr, "oh no\n");
    return -1;
  }

  while (strcmp(msg, "quit\n")) {
    send_message(clientfd, greeting, strlen(greeting));
  }

  return 0;
}

