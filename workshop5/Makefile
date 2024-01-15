CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -masm=intel -O0

all: file_generator wk5_monitor wk5_victim

file_generator: file_generator.c
	$(CC) $(CFLAGS) -o file_generator file_generator.c

wk5_monitor: wk5_monitor.c
	$(CC) $(CFLAGS) -o wk5_monitor wk5_monitor.c
	
wk5_victim: wk5_victim.c
	$(CC) $(CFLAGS) -o wk5_victim wk5_victim.c
	
clean:
	rm -f file_generator wk5_monitor wk5_victim
