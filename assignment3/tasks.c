#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <x86intrin.h>

#define THRESHOLD 150 // timing around 14-16 when cached

static void fNOT(void *out, void *in); // NOT gate
static void fNOR(void *out, void *in1, void *in2);
static void fNAND(void *out, void *in1, void *in2);

static uint64_t probe(void *adrs); // access adrs and return access
static void flush(void *adrs); // clflush adrs 
static void load(void *adrs); // load adrs into cache
static void wait(uint64_t cycles); // just wait

#ifndef TESTCASE
int main(){
	printf("hi\n");
}
#endif


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
		"lea rbx, QWORD PTR [fNOT_2];"
		"call fNOT_1;"
		// BEGIN Spec part 		
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
		// END delay ops
		"mov r11, QWORD PTR [%0+rax];" // spec instr
		"lfence;"
		// END Spec part
		"fNOT_1: mov QWORD PTR [rsp], rbx;" 
		"mov r11, QWORD PTR [%1];" // load input
		"add QWORD PTR [rsp], r11;" // data dependency between input and ptr adrs
		"ret;"
		
		"fNOT_2: nop;"
		: "=r" (out)
		: "r" (in)
		: "rax", "rbx", "r11", "memory"
	);
}

// works like an and logical gate ._.

static void fNOR(void *out, void *in1, void *in2){
	__asm__ volatile(
		"lea rbx, QWORD PTR [fNOR_3];"
		"call fNOR_1;"
		"call fNOR_2;"
		// BEGIN spec part 
		"xor rax, rax;"
		// BEGIN delay ops 
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"		
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"		
		"mov rax, QWORD PTR [rsp+rax];"
		"and rax, 0x0;"
		// END delay ops 
		"mov r11, QWORD PTR [%0+rax];" // addr output + 0
		// END spec part 
		// first input 
		"fNOR_1: mov QWORD PTR [rsp], rbx;"
		"mov r11, [%1];"
		"add [rsp], r11;"
		"ret;"
		//second input 
		"fNOR_2: mov QWORD PTR [rsp], rbx;"
		"mov r11, [%2];"
		"add [rsp], r11;"
		"ret;"
		// end 
		"fNOR_3: nop;"
		: "=r" (out)
		: "r" (in1), "r" (in2)
		: "rax", "rbx", "r11", "memory"
	);
	
	return; // TODO
}



// works lika an or logic gate!

static void fNAND(void *out, void *in1, void *in2){
	__asm__ volatile(
		"lea rbx, QWORD PTR [fNAND_2];"
        "call fNAND_1;"
        "xor rax, rax;"
        "mov rax, QWORD PTR [rsp+rax];"
        "and rax, 0x0;"
        "mov r11, QWORD PTR [%0+rax];"
        "lfence;"
        "fNAND_1: mov QWORD PTR [rsp], rbx;"
        "mov r11, QWORD PTR [%1];"
        "add r11, QWORD PTR [%2];"
        "add QWORD PTR [rsp], r11;"
        "ret;"
        "fNAND_2: nop;"
        : "=r" (out)
        : "r" (in1), "r" (in2)
        : "rax", "rbx", "r11", "memory"
    );
}













