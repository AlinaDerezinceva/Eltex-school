CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -ldl
TARGET = calculator
SRCS = main.c menu.c plugins.c
LIB_DIR = libs

all: $(TARGET) prepare_libs

prepare_libs:
	mkdir -p $(LIB_DIR)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

%.so: %.c
	$(CC) $(CFLAGS) -shared -fPIC -o $(LIB_DIR)/$@ $<

clean:
	rm -f $(TARGET)
	rm -rf $(LIB_DIR)

.PHONY: all clean prepare_libs