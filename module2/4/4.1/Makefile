CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

TARGET = contacts
SRCS = main.c actions.c interface.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean