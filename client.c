#include "client.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
int Start(char *ip, int port, char *message) {
  int status, client_fd;
  (void)message; // unused
  struct sockaddr_in server_address;
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
  int shouldStop = 0;
  ThreadArgs targs = {.socket = client_fd, .shouldStop = &shouldStop};
  pthread_t listener, sender;
  pthread_create(&sender, NULL, (void *(*)(void *))Sender, (void *)&targs);
  pthread_create(&listener, NULL, (void *(*)(void *))Listener, (void *)&targs);
  pthread_join(sender, NULL);
  pthread_join(listener, NULL);
  close(client_fd);
  return 0;
}

void *Listener(void *args) {
  ThreadArgs *targs = (ThreadArgs *)args;
  int *shouldStop = targs->shouldStop;
  int socket = targs->socket;
  char buffer[1024] = {0};
  ssize_t numBytesRead = 0;
  while (*shouldStop != 1) {
    memset(buffer, 0, 1024);
    numBytesRead = read(socket, buffer, 1024);
    if (numBytesRead < 0) {
      perror("ERROR reading from socket");
      break;
    }
    if (strlen(buffer) > 1 && buffer[0] == '/' && buffer[1] == 'q') {
      printf("Server closed connection\n");
      char *retBuf = "/r";
      send(socket, retBuf, strlen(retBuf), 0);
      *shouldStop = 1;
      return NULL;
    }
    if (strlen(buffer) > 1 && buffer[0] == '/' && buffer[1] == 'r') {
      *shouldStop = 1;
      return NULL;
    }
    printf("Received: %s", buffer);
  }
  *shouldStop = 1;
  return NULL;
}
void *Sender(void *args) {
  ThreadArgs *targs = (ThreadArgs *)args;
  int *shouldStop = targs->shouldStop;
  int socket = targs->socket;
  char message[256] = {0};
  while (*shouldStop != 1) {
    if (fgets(message, 256, stdin) == NULL) {
      printf("ERROR reading message\n");
      *shouldStop = 1;
      return NULL;
    }
    if (strlen(message) > 1 && message[0] == '/' && message[1] == 'q') {
      printf("Quitting\n");
      *shouldStop = 1;
      send(socket, message, strlen(message), 0);
      return NULL;
    }
    send(socket, message, strlen(message), 0);
  }
  return NULL;
}
