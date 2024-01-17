#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <x86intrin.h>

#define THRESHOLD 160 // timing around 14-16 when cached

static void fNOT(void *out, void *in); // NOT gate
static void fNOTX(void *out, void *in, uint64_t x); // xNOT gate with x out
static void fNOR(void *out, void *in1, void *in2);
static void fNAND(void *out, void *in1, void *in2);
static void fAND(void *out, void *in1, void *in2);
static void fOR(void *out, void *in1, void *in2);

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
	else if (x==1) fNOT(out, in);
	else {
		__asm__ volatile(
			"lea rbx, [fNOT2_2];"
			"call fNOT2_1;"
			"xor rax, rax;"
			// BEGIN delay ops 
			".rept 5;" // deplen
			"mov rax, [rsp+rax];"
			"and rax, 0x0;"
			".endr;"
			// BEGIN Spec part
			".rept 5;" // accesslen
			"mov r11, [rdi+rax];" // spec instr
			"add rdi, 0x1040;" // add stride 4160 = 0x1040
			".endr;"
			"lfence;"
			// END Spec part
			"fNOT2_1: mov [rsp], rbx;" 
			"mov r11, [rsi];" // load input
			"add [rsp], r11;" // data dependency between input and ptr adrs
			"ret;"
			
			"fNOT2_2: nop;"
			: 
			: "S" (in), "D" (out), "r" (out+4160)
			: "rax", "rbx", "r11", "memory"
		);
	}
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

static void fOR(void *out, void *in1, void *in2){
	__asm__ volatile(
		"lea rbx, [fOR_2];"
        "call fOR_1;"
		// BEGIN spec code
		"mov rax, [%1];" // in1
        "add r11, [%0+rax];" // out
		// END spec code
	
        "lfence;"
		
        "fOR_1: mov [rsp], rbx;" // move 
		"xor rax, rax;"
		".rept 50;"
        "mov rax, [rsp+rax];"
        "and rax, 0x0;"
		".endr;"
        "add [rsp], rax;"
        "ret;"
        
		"fOR_2: lea rbx, [fOR_4];"
		"call fOR_3;" // end
		// BEGIN spec code
		"mov rax, [%2];" // in2
        "add r11, [%0+rax];" // out
		// END spec code
		"lfence;"
		"fOR_3: mov [rsp], rbx;" // move 
		"xor rax, rax;"
		".rept 50;"
        "mov rax, [rsp+rax];"
        "and rax, 0x0;"
		".endr;"
        "add [rsp], rax;"
        "ret;"
		
		"fOR_4: nop;"
        : 
        : "r" (out), "r" (in1), "r" (in2)
        : "rax", "rbx", "r11", "memory"
    );
}


// ~ 99 %
static void fAND(void *out, void *in1, void *in2){
	__asm__ volatile(
		"lea rbx, [fAND_2];"
        "call fAND_1;"
		// BEGIN spec code
		"mov rax, [%1];" // in1
        "add rax, [%2];" // in2
		"add r11, [%0+rax];"
		// END spec code
        "lfence;"
        "fAND_1: mov [rsp], rbx;" // move 
		"xor rax, rax;"
		".rept 50;"
        "mov rax, [rsp+rax];"
        "and rax, 0x0;"
		".endr;"
        "add [rsp], rax;"
        "ret;"
        "fAND_2: nop;" // end
        : 
        : "r" (out), "r" (in1), "r" (in2)
        : "rax", "rbx", "r11", "memory"
    );
}











