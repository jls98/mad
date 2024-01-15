CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -masm=intel -O0

all: file_generator a1t1 a1t2 a1t3 monitor fr_trace madgpg_monitor

file_generator: file_generator.c
	$(CC) $(CFLAGS) -o file_generator file_generator.c

a1t1: a1t1.c
	$(CC) $(CFLAGS) -o a1t1 a1t1.c
	
a1t2: a1t2.c
	$(CC) $(CFLAGS) -o a1t2 a1t2.c
	
a1t3: a1t3.c
	$(CC) $(CFLAGS) -o a1t3 a1t3.c
	
monitor: monitor.c
	$(CC) $(CFLAGS) -o monitor monitor.c 

fr_trace: fr_trace.c 	
	$(CC) $(CFLAGS) -o fr_trace fr_trace.c 
	
madgpg_monitor: madgpg_monitor.c 
	$(CC) $(CFLAGS) -o madgpg_monitor madgpg_monitor.c 
	
clean:
	rm -f file_generator a1t1 a1t2 a1t3 monitor fr_trace madgpg_monitor
