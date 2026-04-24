CC     = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = pq_test
SRCS   = main.c priority_queue.c
HEADERS = priority_queue.h structures.h

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)