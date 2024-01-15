#to tell from where to include libraries
LIB = -L $(HOME)/usr/lib
#to tell where to find include files
INC = -I $(HOME)/usr/include
CC = gcc
CFLAGS = -Wall -Wextra -masm=intel $(INC) $(LIB) 



all: workshop7

workshop7: workshop7.c
	$(CC) $(CFLAGS) -o workshop7 workshop7.c -lassemblyline

	
clean:
	rm -f workshop7
