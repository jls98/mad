#include <assemblyline.h> // all assemblyline functions
#include <stdint.h>       // uint8_t
#include <stdio.h>        // printf
#include <sys/mman.h>     // mmap
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int foo(int a)
{
	const int buffer_size = 300; // bytes
	uint8_t *mybuffer = mmap(NULL, sizeof(uint8_t) * buffer_size,
                           PROT_READ | PROT_WRITE | PROT_EXEC,
                           MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

	assemblyline_t al = asm_create_instance(mybuffer, buffer_size);
	
	// negate input
	// a and -a
	asm_assemble_str(al, "mov rax, rdi");
	asm_assemble_str(al, "neg rdi");
	asm_assemble_str(al, "and rax, rdi");
	asm_assemble_str(al, "ret");

	int (*func)() = asm_get_code(al);

	int result = func();
	return result;
}

int main(int ac, char **av) {
	if (ac==2){
		printf("starting\n");
		printf("%i\n",foo(atoi(av[1])));
	} 
	return 0;
}
