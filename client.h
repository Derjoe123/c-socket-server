
typedef struct ThreadArgs {
  int socket;
  int *shouldStop;
} ThreadArgs;

int Start(char *ip, int port, char *message);

void *Listener(void *args);
void *Sender(void *args);
