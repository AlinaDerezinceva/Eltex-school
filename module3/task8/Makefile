CC = gcc
CFLAGS = -Wall -std=c11 
TARGETS = producer consumer

.PHONY: all clean

all: $(TARGETS)

producer: producer.c 
	$(CC) $(CFLAGS) -o $@ $<

consumer: consumer.c 
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -fv $(TARGETS) *.o *.pos *.dat *.txt