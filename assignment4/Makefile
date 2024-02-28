CC = gcc
CFLAGS = -Wall -Wextra -masm=intel -O0

all: meltdown-template test_meltdown-template file_loader

file_loader: file_loader.c 
    $(CC) $(CFLAGS) -o file_loader file_loader.c
    
meltdown-template: meltdown-template.c
	$(CC) $(CFLAGS) -o meltdown-template -DMELTDOWNCASE meltdown-template.c

test_meltdown-template: test_meltdown-template.c 
	$(CC) $(CFLAGS) -o test_meltdown-template -DTESTCASE  test_meltdown-template.c meltdown-template.c -lcunit
    
clean:
	rm -f meltdown-template test_meltdown-template file_loader
