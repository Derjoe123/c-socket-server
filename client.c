#include "client.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int Start(char *ip, int port, char *message) {
  int status, client_fd;
  struct sockaddr_in server_address;
  char buffer[1024] = {0};
  ssize_t numBytesRead = 0;
  client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd < 0) {
    perror("ERROR opening socket");
    return 1;
  }
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);

  if (inet_pton(AF_INET, ip, &server_address.sin_addr) <= 0) {
    perror("ERROR invalid address");
    return 1;
  }
  status = connect(client_fd, (struct sockaddr *)&server_address,
                   sizeof(server_address));
  if (status < 0) {
    perror("ERROR connecting");
    return 1;
  }
  send(client_fd, message, strlen(message), 0);
  printf("Message sent\n");
  numBytesRead = read(client_fd, buffer, 1024);
  if (numBytesRead < 0) {
    perror("ERROR reading from socket");
    return 1;
  }
  printf("%s\n", buffer);
  close(client_fd);
  return 0;
}
