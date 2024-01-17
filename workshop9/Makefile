CC = gcc
CFLAGS = -Wall -Wextra -masm=intel

all: victim spectre

	
victim: victim.c 
	$(CC) $(CFLAGS) -o victim victim.c 

spectre: spectre.c 
	$(CC) $(CFLAGS) -o spectre spectre.c 
	
clean:
	rm -f victim spectre