#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// If we're on WASM, use the custom implementation, else stick to the
// default includes
#ifdef IS_WASM
#include "berkeley_sockets.h"
#endif

#define REMOTE_ADDR "127.0.0.1"
#define REMOTE_PORT 6912

#define SENT_MESSAGE_MAX_LENGTH 1024
#define RECEIVED_MESSAGE_PREFIX "You've sent: "
#define RECEIVED_MESSAGE_MAX_LENGTH                                            \
  SENT_MESSAGE_MAX_LENGTH + sizeof(RECEIVED_MESSAGE_PREFIX)

int main() {
  // Variables
  int remote_sock;
  struct sockaddr_in remote_addr;

  ssize_t sent_message_length;
  char sent_message[SENT_MESSAGE_MAX_LENGTH];
  size_t received_message_length;
  char received_message[RECEIVED_MESSAGE_MAX_LENGTH];

  socklen_t remote_addr_length = sizeof(struct sockaddr_in);

  memset(&remote_addr, 0, sizeof(remote_addr));

  // Create address to connect to
  remote_addr.sin_family = AF_INET;
  remote_addr.sin_port = htons(REMOTE_PORT);
  if (inet_pton(AF_INET, REMOTE_ADDR, &remote_addr.sin_addr) == -1) {
    perror("inet_pton");

    exit(-1);
  }

  // Create socket
  if ((remote_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");

    exit(-1);
  }

  // Connect
  if ((connect(remote_sock, (struct sockaddr *)&remote_addr,
               remote_addr_length)) == -1) {
    perror("connect");

    exit(-1);
  }

  while (1) {
    memset(&received_message, 0, RECEIVED_MESSAGE_MAX_LENGTH);
    memset(&sent_message, 0, SENT_MESSAGE_MAX_LENGTH);

    fgets(sent_message, SENT_MESSAGE_MAX_LENGTH, stdin);

    printf("%lu\n", strlen(sent_message));

    // Send
    sent_message_length =
        send(remote_sock, sent_message, strlen(sent_message), 0);

    // Receive
    received_message_length =
        recv(remote_sock, &received_message, RECEIVED_MESSAGE_MAX_LENGTH, 0);

    printf("%s", received_message);
  }

  return 0;
}