#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <x86intrin.h>

#define THRESHOLD 160 // timing around 14-16 when cached

static void fNOT(void *out, void *in); // NOT gate
static void fNOTX(void *out, void *in, uint64_t x); // xNOT gate with x out
static void fNOR(void *out, void *in1, void *in2);
static void fNAND(void *out, void *in1, void *in2);

static uint64_t probe(void *adrs); // access adrs and return access
static void flush(void *adrs); // clflush adrs 
static void load(void *adrs); // load adrs into cache
static void wait(uint64_t cycles); // just wait
static void fence();

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
	__asm__ volatile("mov rax, [%0];"::"r" (adrs): "rax", "memory");
}

static void flush(void *adrs){
	__asm__ volatile("clflush [%0];lfence" ::"r" (adrs));
}

static void fence(){
	__asm__ volatile("mfence;lfence");
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

static void fNOT(void *out, void *in){
	
	__asm__ volatile(
		"lea rbx, [fNOT_2];"
		"call fNOT_1;"
		// BEGIN Spec part 		
		"xor rax, rax;"
		// BEGIN delay ops
		".rept 5;"
		"mov rax, [rsp+rax];"
		"and rax, 0x0;"
		".endr;"
		// END delay ops
		"mov r11, [rdi+rax];" // spec instr
		"lfence;"
		// END Spec part
		"fNOT_1: mov [rsp], rbx;" 
		"mov r11, [rsi];" // load input
		"add [rsp], r11;" // data dependency between input and ptr adrs
		"ret;"
		"fNOT_2: nop;"
		: 
		: "D" (out), "S" (in)
		: "rax", "rbx", "r11", "memory"
	);
}

static void fNOTX(void *out, void *in, uint64_t x){	
	if (x==0) return;
	__asm__ volatile(
		"lea rbx, [fNOTX_2];"
		"mov rcx, %2;" // x
		"mov r8, 4160;"
		"mov rdx, %0;"
		"call fNOTX_1;"
		"xor rax, rax;"
		// BEGIN delay ops 
		".rept 1;"
		"mov rax, [rsp+rax];"
		"and rax, 0x0;"
		".endr;"
		// BEGIN Spec part 		
		"loop: mov r11, [rdx+rax];" // spec instr
		"dec rcx;"
		"add rdx, r8;"
		"jnz loop;"
		"lfence;"
		// END Spec part
		"fNOTX_1: mov [rsp], rbx;" 
		"mov r11, [%1];" // load input
		"add [rsp], r11;" // data dependency between input and ptr adrs
		"ret;"
		
		"fNOTX_2: nop;"
		: 
		: "r" (out), "r" (in), "r" (x)
		: "rax", "rbx", "rcx", "rdx", "r8", "r11", "memory"
	);
}

static void fNOT3(void *out1, void *out2, void *out3, void *in){	
	__asm__ volatile(
		"lea rbx, [fNOT3_2];"
		"call fNOT3_1;"
		// BEGIN Spec part 		
		"xor rax, rax;"
		// BEGIN delay ops
		".rept 5;"
		"mov rax, [rsp+rax];"
		"and rax, 0x0;"
		".endr;"
		// END delay ops
		"mov r11, [%0+rax];" // spec instr
		"mov r11, [%1+rax];" // spec instr
		"mov r11, [%2+rax];" // spec instr
		"lfence;"
		// END Spec part
		"fNOT3_1: mov [rsp], rbx;" 
		"mov r11, [%3];" // load input
		"add [rsp], r11;" // data dependency between input and ptr adrs
		"ret;"
		
		"fNOT3_2: nop;"
		: 
		: "r" (out1), "r" (out2), "r" (out3), "r" (in)
		: "rax", "rbx", "r11", "memory"
	);
}

static void fNOT4(void *out1, void *out2, void *out3, void *out4, void *in){	
	__asm__ volatile(
		"lea rbx, [fNOT4_2];"
		"call fNOT4_1;"
		// BEGIN Spec part 		
		"xor rax, rax;"
		// BEGIN delay ops
		".rept 5;"
		"mov rax, [rsp+rax];"
		"and rax, 0x0;"
		".endr;"
		// END delay ops
		"mov r11, [%0+rax];" // spec instr
		"mov r11, [%1+rax];" // spec instr
		"mov r11, [%2+rax];" // spec instr
		"mov r11, [%3+rax];" // spec instr
		"lfence;"
		// END Spec part
		"fNOT4_1: mov [rsp], rbx;" 
		"mov r11, [%4];" // load input
		"add [rsp], r11;" // data dependency between input and ptr adrs
		"ret;"
		
		"fNOT4_2: nop;"
		: 
		: "r" (out1), "r" (out2),"r" (out3), "r" (out4), "r" (in)
		: "rax", "rbx", "r11", "memory"
	);
}


static void fNOR(void *out, void *in1, void *in2){
	__asm__ volatile(
		"lea rbx, [fNOR_3];"
		"call fNOR_1;"
		"call fNOR_2;"
		// BEGIN spec part 
		"xor rax, rax;"
		// BEGIN delay ops 
		".rept 5;"
		"mov rax, [rsp+rax];"
		"and rax, 0x0;"	
		".endr;"		
		// END delay ops 
		"mov r11, [%0+rax];" // addr output + 0
		"lfence;"
		// END spec part 
		"fNOR_1: mov [rsp], rbx;"		// in1
		"mov r11, [%1];"
		"add [rsp], r11;"
		"ret;"
		
		"fNOR_2: mov [rsp], rbx;"		// in2 
		"mov r11, [%2];"
		"add [rsp], r11;"
		"ret;"
		// end 
		"fNOR_3: nop;"
		: 
		: "r" (out), "r" (in1), "r" (in2)
		: "rax", "rbx", "r11", "memory"
	);
}

static void fNAND(void *out, void *in1, void *in2){
	__asm__ volatile(
		"lea rbx, [fNAND_2];"
        "call fNAND_1;"
		// BEGIN spec code
        "xor rax, rax;"
		".rept 20;"
        "mov rax, [rsp+rax];"
        "and rax, 0x0;"
		".endr;"
		"mov r11, [%0+rax];" // out
		// END spec code
        "lfence;"
        "fNAND_1: mov [rsp], rbx;" // move 
        "mov r11, [%1];" // in1
        "add r11, [%2];" // in2
        "add [rsp], r11;"
        "ret;"
        "fNAND_2: nop;"
        : 
        : "r" (out), "r" (in1), "r" (in2)
        : "rax", "rbx", "r11", "memory"
    );
}













