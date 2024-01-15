#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <x86intrin.h>

void fNOT(void *out, void *in); // NOT gate
uint64_t probe(void *adrs); // access adrs and return access
void flush(void *adrs); // clflush adrs 
void load(void *adrs); // load adrs into cache
void wait(uint64_t cycles); // just wait
#define THRESHOLD 150 // timing around 14-16 when cached


void test(){
	// preparation
	uint64_t *a = (uint64_t *) malloc(sizeof(uint64_t *));
	uint64_t *b = (uint64_t *) malloc(sizeof(uint64_t *));
	*a=0;
	*b=0;
	wait(1E9);
	uint64_t time, ctr_notA=0, ctr_A=0;
	
	for (int i=0;i<100000;i++){

		wait(1E9);
		flush(a);
		flush(b);

		// not A
		fNOT(b, a);
		time = probe(b);
		if (time<THRESHOLD) ctr_notA++;

		wait(1E9);
		flush(a);
		flush(b);
		load(a);

		// A
		fNOT(b, a);
		time = probe(b);
		if (time>THRESHOLD) ctr_A++;
	
	}
	printf("A results in B=%lu, time %lu\n", ctr_A, time);
	printf("not A results in B=%lu, time %lu\n", ctr_notA, time);

}

int main(){
	
test();	
}

void wait(uint64_t cycles) {
	unsigned int ignore;
	uint64_t start = __rdtscp(&ignore);
	while (__rdtscp(&ignore) - start < cycles);
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
		
		"mov rsi, %1;"
		"mov rdi, %0;"
		"call label_1;"
		"xor rax, rax;"
		// BEGIN delay ops
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"
		// END delay ops
		"mov r11, QWORD PTR [rdi+rax];" // spec instr
		"lfence;"
		"label_1: lea rax, [label_2];" 
		"mov QWORD PTR [rsp], rax;"
		"mov r11, QWORD PTR [rsi];"
		"add QWORD PTR [rsp], r11;"
		"ret;"
		"label_2: nop;"
		: "=r" (out)
		: "r" (in)
		: "rax", "rdi", "rsi", "r11", "memory"
	);
		
	
	
	
	
	
	return; // TODO
}