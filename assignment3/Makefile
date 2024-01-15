CC = gcc
CFLAGS = -Wall -Wextra -masm=intel -O0 -no-pie


all: tasks

tasks: tasks.c
	$(CC) $(CFLAGS) -o tasks tasks.c
	
clean:
	rm -f tasks
