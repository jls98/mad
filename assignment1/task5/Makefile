CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -masm=intel

all: file_generator fr_trace

file_generator: file_generator.c
	$(CC) $(CFLAGS) -o file_generator file_generator.c

fr_trace: fr_trace.c
	$(CC) $(CFLAGS) -o fr_trace fr_trace.c

clean:
	rm -f file_generator fr_trace
