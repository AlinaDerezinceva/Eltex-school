CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

all: server client

server: server.o
	$(CC) $(CFLAGS) -o $@ $^

client: client.o
	$(CC) $(CFLAGS) -o $@ $^

server.o: server.c
	$(CC) $(CFLAGS) -c $< -o $@

client.o: client.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o server client

.PHONY: all clean