CC = gcc
CFLAGS = -Wall -Wextra -masm=intel -O0

all: workshop3 file_generator

workshop3: workshop3.c
	$(CC) $(CFLAGS) -o workshop3 workshop3.c
	
file_generator: file_generator.c
	$(CC) $(CFLAGS) -o file_generator file_generator.c

clean:
	rm -f workshop3 file_generator
