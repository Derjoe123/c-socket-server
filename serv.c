#include "serv.h"
#include <arpa/inet.h>
#include <dirent.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int Start(int port) {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  char *hello = "Hello from server";
  ssize_t numBytesRead = 0;
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    perror("[C.O.C] ERROR opening socket");
    exit(1);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) {
    perror("[C.O.C] ERROR opening socket");
    exit(1);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("[C.O.C] ERROR on binding");
    exit(1);
  }
  printf("[C.O.C] Listening on port %d\n", port);
  if (listen(server_fd, 3) < 0) {
    perror("[C.O.C] ERROR on listen");
    exit(1);
  }

  new_socket =
      accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

  if (new_socket < 0) {
    perror("[C.O.C] ERROR on accept");
    exit(1);
  }
  numBytesRead = read(new_socket, buffer, 1024);
  if (numBytesRead < 0) {
    perror("[C.O.C] ERROR reading from socket");
    exit(1);
  }
  printf("%s\n", buffer);
  send(new_socket, hello, strlen(hello), 0);
  printf("[C.O.C] Hello message sent\n");
  close(new_socket);
  close(server_fd);
  return 0;
}
