

all: client.c edge.c server_or.c server_and.c 
	gcc -o client client.c
	gcc -o edge edge.c
	gcc -o server_or server_or.c
	gcc -o server_and server_and.c

.PHONY:client edge server_or server_and
edge:
	./edge
server_or:
	./server_or
server_and:
	./server_and


