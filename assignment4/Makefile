CC = gcc
CFLAGS = -Wall -Wextra -masm=intel -O0


all: meltdown-template

meltdown-template: meltdown-template.c
	$(CC) $(CFLAGS) -o meltdown-template meltdown-template.c
	
clean:
	rm -f meltdown-template
