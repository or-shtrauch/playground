CC = gcc
AR = ar
CFLAGS = -Wall -Werror -O2 -I./include
TARGET = libcool.a
OBJECTS = cool.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

cool.o: src/cool.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean