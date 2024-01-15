#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void fNOT(void *out, void *in); // NOT gate
uint64_t probe(void *adrs); // access adrs and return access
void flush(void *adrs); // clflush adrs 
void load(void *adrs); // load adrs into cache

#define THRESHOLD 200 // timing around 14-16 when cached

int main(){
	
	// preparation
	uint64_t *a = (uint64_t *) malloc(sizeof(uint64_t *));
	uint64_t *b = (uint64_t *) malloc(sizeof(uint64_t *));
	*a=0;
	*b=0;
	flush(a);
	flush(b);

	// not A
	fNOT(b, a);
	uint64_t time = probe(b);
	printf("not A results in B=%lu\n", time);
	
	
	flush(a);
	flush(b);
	
	// A
	/*load(a);
	fNOT(b, a);
	time = probe(b);
	printf("A results in B=%lu\n", time);
	*/
	
	
	
	
}

void load(void *adrs){
	__asm__ volatile("mov rax, [%0];"::"r" (adrs): "rax");
}

void flush(void *adrs){
	__asm__ volatile("clflush [%0];" ::"r" (adrs));
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
        : "r" (adrs)
        : "rdx", "r8", "memory"
	);
	return time;
}


void fNOT(void *out, void *in){
	__asm__ volatile(
		"call fir;"
		"xor rax, rax;"
		"nop; nop; nop; nop; nop;  # Delay ops"
		"mov rax, [rsp+rax];"
		"and rax, 0;"
		"mov r11, [%0+rax];"
		"lfence;"
		"fir:" 
		"mov [rsp], sec;"
		"mov r11, [%1];"
		"add [rsp], r11;"
		"ret;"
		"sec:" 
		"nop;"

		: "=r" (out)
		: "r" (in)
		: "rax", "r11", "memory"
	);
		
	
	
	
	
	
	return; // TODO
}