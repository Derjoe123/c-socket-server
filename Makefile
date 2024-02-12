flags = -Wall -Wextra -Werror -std=c99 -pedantic -lpthread -O3
servobj = main.c serv.c
server: $(servobj)
	gcc $(flags) -o server $(servobj)
clientobj = clientmain.c client.c
client: $(clientobj)
	gcc $(flags) -o client $(clientobj)

make all: server client
