#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <x86intrin.h>

#define THRESHOLD 150 // timing around 14-16 when cached

static void fNOT(void *out, void *in); // NOT gate
static void fNOR(void *out, void *in1, void *in2);
static void NAND(void *out, void *in1, void *in2);

static uint64_t probe(void *adrs); // access adrs and return access
static void flush(void *adrs); // clflush adrs 
static void load(void *adrs); // load adrs into cache
static void wait(uint64_t cycles); // just wait
static int fibonacci(int n);
static void clear_cache();
#ifndef TESTCASE
int main(){
	printf("hi\n");
}
#endif


static int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

static void wait(uint64_t cycles) {
	unsigned int ignore;
	uint64_t start = __rdtscp(&ignore);
	while (__rdtscp(&ignore) - start < cycles);
}

static void load(void *adrs){
	__asm__ volatile("mov rax, [%0];"::"r" (adrs): "rax");
}

static void flush(void *adrs){
	__asm__ volatile("clflush [%0];" ::"r" (adrs));
}

static void clear_cache(){
	uint64_t *dump = (uint64_t *) malloc(512*sizeof(uint64_t));
	for(int i=0; i<512;i++){
		if(i>0) dump[i] = 1;
	}
	free(dump);
}

static uint64_t probe(void *adrs){
	volatile uint64_t time;  
	__asm__ volatile (
        " mfence            \n"
        " lfence            \n"
        " rdtscp             \n"
        " mov r8, rax 		\n"
        " mov rax, [%1]		\n"
        " lfence            \n"
        " rdtscp             \n"
        " sub rax, r8 		\n"
        : "=&a" (time)
        : "r" (adrs)
        : "ecx", "rdx", "r8", "memory"
	);
	return time;
}

// works 50 % of the time lol
static void fNOT(void *out, void *in){
	
	__asm__ volatile(
		//"mov rsi, %1;"
		//"mov rdi, %0;"
		"lea rbx, [label_2];"
		"call label_1;"
		"xor rax, rax;"
		// BEGIN delay ops
		"mov rax, QWORD PTR [rsp+rax];"
		/*"and rax, 0x0;"
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"*/
		// END delay ops
		"mov r11, QWORD PTR [%0+rax];" // spec instr
		"lfence;"
		"label_1: mov QWORD PTR [rsp], rbx;" 
		"mov r11, QWORD PTR [%1];"
		"add QWORD PTR [rsp], r11;"
		"ret;"
		"label_2: nop;"
		: "=r" (out)
		: "r" (in)
		: "rax", "rbx", "r11", "memory"
	);
}
/*

static void fNOR(void *out, void *in1, void *in2){
	
}




static void NAND(void *out, void *in1, void *in2){
	__asm__ volatile(
		"mov "
		"lea rbx, [label_4];"
        "call label_3;"
        "xor rax, rax;"
        "mov rax, QWORD PTR [rsp+rax*1];"
        "and rax, 0x0;"
        "mov rax, QWORD PTR [rsp+rax*1];"
        "and rax, 0x0;"
        "mov rax, QWORD PTR [rsp+rax*1];"
        "and rax, 0x0;"
        "mov rax, QWORD PTR [rsp+rax*1];"
        "and rax, 0x0;"
        "mov rax, QWORD PTR [rsp+rax*1];"
        "and rax, 0x0;"
        "mov r11, QWORD PTR [rdi+rax*1];"
        "lfence;"
        "label_3: mov QWORD PTR [rsp], rbx;"
        "mov r11, QWORD PTR [rsi];"
        "add r11, QWORD PTR [rdx];"
        "add QWORD PTR [rsp], r11;"
        "ret;"
        "label_4: nop;"
        : "=r" (*out)
        : "r" (in1), "r" (in2)
        : "rax", "rbx", "rsi", "rdx", "rdi", "r11", "memory"
    );
}

*/











