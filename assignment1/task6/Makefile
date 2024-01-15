CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -masm=intel

all: file_generator madgpg_monitor

file_generator: file_generator.c
	$(CC) $(CFLAGS) -o file_generator file_generator.c

madgpg_monitor: madgpg_monitor.c
	$(CC) $(CFLAGS) -o madgpg_monitor madgpg_monitor.c

clean:
	rm -f file_generator madgpg_monitor
