CC = gcc
CFLAGS = -Wall -Wextra -masm=intel -O0 


all: tasks test_tasks

tasks: tasks.c
	$(CC) $(CFLAGS) -o tasks tasks.c
	
test_tasks: test_tasks.c 
	$(CC) $(CFLAGS) -o test_tasks -DTESTCASE  test_tasks.c tasks.c -lcunit
    
clean:
	rm -f tasks test_tasks
