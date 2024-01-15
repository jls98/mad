CC = gcc
CFLAGS = -Wall -Wextra -masm=intel

all: madgpg_monitor

	
madgpg_monitor: madgpg_monitor.c 
	$(CC) $(CFLAGS) -o madgpg_monitor madgpg_monitor.c 
	
clean:
	rm -f madgpg_monitor
