CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: file_generator a1t1

file_generator: file_generator.c
	$(CC) $(CFLAGS) -o file_generator file_generator.c

a1t1: a1t1.c
	$(CC) $(CFLAGS) -o a1t1 a1t1.c

clean:
	rm -f file_generator a1t1
