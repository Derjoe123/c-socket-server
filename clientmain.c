#include "client.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <ip> <port> <message>\n", argv[0]);
    return 1;
  }
  Start(argv[1], atoi(argv[2]), argv[3]);
  return 0;
}
