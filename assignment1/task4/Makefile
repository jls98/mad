CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -masm=intel

all: file_generator monitor

file_generator: file_generator.c
	$(CC) $(CFLAGS) -o file_generator file_generator.c

monitor: monitor.c
	$(CC) $(CFLAGS) -o monitor monitor.c

clean:
	rm -f file_generator monitor
