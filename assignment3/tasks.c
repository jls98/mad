#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void fNOT(void *out, void *in); // NOT gate
uint64_t probe(void *adrs); // access adrs and return access
void flush(void *adrs); // clflush adrs 


#define THRESHOLD 200 // timing around 14-16 when cached

int main(){
	
	// preparation
	uint64_t *a = (uint64_t *) malloc(sizeof(uint64_t *));
	uint64_t *b = (uint64_t *) malloc(sizeof(uint64_t *));

	flush(a);
	flush(b);
	
	
	
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
        : "rdx", "r8");
		return time;
}


void fNOT(void *out, void *in){
	__asm__ volatile(
		"1: call 3f;"
		"2: xor rax, rax;"
		"rep; nop; nop; nop; nop; nop;  # Delay ops"
		"mov rax, [rsp+rax];"
		"and rax, 0;"
		".endr;"
		"mov r11, [rdi+rax];"
		"lfence;"
		"3: mov [rsp], 4f;"
		"mov r11, [rsi];"
		"add [rsp], r11;"
		"ret;"
		"4: nop;"

		: "=d" (out)
		: "s" (in)
		: "r11"
	);
		
	
	
	
	
	
	return; // TODO
}