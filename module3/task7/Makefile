# Компилятор и флаги
CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11 -g
LDLIBS  = -lrt

TARGETS = chat_user1 chat_user2

all: $(TARGETS)

chat_user1: chat_user1.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

chat_user2: chat_user2.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

clean:
	rm -f $(TARGETS) *.o

run: all

.PHONY: all clean run