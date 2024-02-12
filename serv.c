#include "serv.h"
#include <arpa/inet.h>
#include <dirent.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int shouldStop = 0;
int Start(int port) {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) {
    perror("ERROR opening socket");
    exit(1);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("ERROR on binding");
    exit(1);
  }
  printf("Listening on port %d\n", port);
  if (listen(server_fd, 3) < 0) {
    perror("ERROR on listen");
    exit(1);
  }

  new_socket =
      accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

  if (new_socket < 0) {
    perror("ERROR on accept");
    exit(1);
  }
  ThreadArgs targs = {.socket = new_socket, .shouldStop = &shouldStop};
  pthread_t listener, sender;
  pthread_create(&sender, NULL, (void *(*)(void *))Sender, (void *)&targs);
  pthread_create(&listener, NULL, (void *(*)(void *))Listener, (void *)&targs);
  pthread_join(sender, NULL);
  pthread_join(listener, NULL);
  close(new_socket);
  close(server_fd);
  return 0;
}

void *Listener(void *args) {
  ThreadArgs *targs = (ThreadArgs *)args;
  int socket = targs->socket;
  char buffer[1024] = {0};
  ssize_t numBytesRead = 0;
  while (1) {
    memset(buffer, 0, 1024);
    numBytesRead = read(socket, buffer, 1024);
    if (numBytesRead < 0) {
      perror("ERROR reading from socket");
      exit(1);
    }
    if (numBytesRead == 0) {
      printf("no data\n");
      continue;
    }
    if (shouldStop == 1) {
      break;
    }
    if (strlen(buffer) > 1 && buffer[0] == '/' && buffer[1] == 'q') {
      printf("User disconnected\n");
      char *retBuf = "/r";
      send(socket, retBuf, strlen(retBuf), 0);
      break;
    }
    if (strlen(buffer) > 1 && buffer[0] == '/' && buffer[1] == 'r') {
      shouldStop = 1;
      char *retBuf = "/r";
      send(socket, retBuf, strlen(retBuf), 0);
      return NULL;
    }
    printf("Received: %s", buffer);
  }
  shouldStop = 1;
  return NULL;
}
void *Sender(void *args) {
  ThreadArgs *targs = (ThreadArgs *)args;
  int socket = targs->socket;
  char message[256] = {0};
  while (1) {
    if (shouldStop == 1) {
      break;
    }
    if (fgets(message, 256, stdin) == NULL) {
      printf("ERROR reading message\n");
      exit(1);
    }
    if (strlen(message) > 1 && message[0] == '/' && message[1] == 'q') {
      printf("Quitting\n");
      shouldStop = 1;
      send(socket, message, strlen(message), 0);
      return NULL;
    }
    send(socket, message, strlen(message), 0);
  }
  return NULL;
}
