CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -masm=intel

all: file_generator a1t2

file_generator: file_generator.c
	$(CC) $(CFLAGS) -o file_generator file_generator.c

a1t2: a1t2.c
	$(CC) $(CFLAGS) -o a1t2 a1t2.c

clean:
	rm -f file_generator a1t2
