#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <ip> <port>\n", argv[0]);
    return 1;
  }
  char *message = malloc(256 * sizeof(char));
  printf("Enter message: ");
  if (fgets(message, 256, stdin) == NULL) {
    printf("ERROR reading message\n");
    return 1;
  }
  Start(argv[1], atoi(argv[2]), message);
  return 0;
}
