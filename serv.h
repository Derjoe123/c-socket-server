
typedef struct ThreadArgs {
  int socket;
  int *shouldStop;
} ThreadArgs;

int Start(int port);

void *Listener(void *args);
void *Sender(void *args);
