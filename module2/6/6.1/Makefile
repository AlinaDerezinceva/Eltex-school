CC = gcc
CFLAGS = -std=c99 -Wall -Wextra
AR = ar
ARFLAGS = rcs

OBJ_DIR = obj
LIB_OBJ = $(OBJ_DIR)/list.o
LIB_NAME = libcontacts.a
MAIN_OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/interface.o
TARGET = contacts

.PHONY: all clean run dirs

all: dirs $(LIB_NAME) $(TARGET)

dirs:
	mkdir -p $(OBJ_DIR)

$(LIB_NAME): $(LIB_OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/list.o: list.c list.h structures.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(MAIN_OBJS) $(LIB_NAME)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJS) -L. -lcontacts

$(OBJ_DIR)/main.o: main.c interface.h list.h structures.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/interface.o: interface.c interface.h list.h structures.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(LIB_NAME) $(TARGET)

run: $(TARGET)
	./$(TARGET)