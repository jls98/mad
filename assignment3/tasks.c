#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void fNOT(void *out, void *in); // NOT gate
uint64_t probe(void *adrs); // access adrs and return access
void flush(void *adrs); // clflush adrs 


#define THRESHOLD 80 // TODO

int main(){
	uint64_t *test = (uint64_t *) malloc(sizeof(uint64_t *));
	
	uint64_t timing = probe(test);
	printf("%lu\n", timing);
	*test = 10;
	
	uint64_t timing = probe(test);
	printf("%lu\n", timing);
	flush(test);
	uint64_t timing = probe(test);
	printf("%lu\n", timing);
	
}

void flush(void *adrs){
	__asm__ volatile("clflush %0;" :"r" (adrs));
}
uint64_t probe(void *adrs){
	volatile uint64_t time;  
	__asm__ volatile (
        " mfence            \n"
        " lfence            \n"
        " rdtsc             \n"
        " mov r8, rax 		\n"
        " mov rax, [%1]		\n"
        " lfence            \n"
        " rdtsc             \n"
        " sub rax, r8 		\n"
        : "=&a" (time)
        : "r" (p)
        : "rdx", "r8");
		return time;
}


void fNOT(void *out, void *in){
	return; // TODO
}