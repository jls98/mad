#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <x86intrin.h>

#define THRESHOLD 160 // timing around 14-16 when cached

static void fNOT(void *out, void *in); // NOT gate
static void fNOTN(void* out_1, void* out_2/*, void* out_3, void* out_4*/, void* in);
//static void fNOTX(void *out, void *in, uint64_t x); // xNOT gate with x out
static void fNOR(void *out, void *in1, void *in2);
static void fNORN(void *in1, void *in2, void *out1, void *out2/*, void *out3, void *out4*/);
static void fNAND(void *out, void *in1, void *in2);
static void fNANDN(void *in1, void *in2, void *out1, void *out2/*, void *out3, void *out4*/);
static void fAND(void *out, void *in1, void *in2);
static void fOR(void *out, void *in1, void *in2);

static void fXOR(void *out, void *in1, void *in2, void **buf);

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

/*static void fNOTX(void *out, void *in, uint64_t x){	
	if (x==0) return;
	else if (x==1) fNOT(out, in);
	else {
		__asm__ volatile(
			"lea rbx, [fNOT2_2];"
			"call fNOT2_1;"
			"xor rax, rax;"
			// BEGIN delay ops 
			".rept 15;" // deplen
			"mov rax, [rsp+rax];"
			"and rax, 0x0;"
			".endr;"
			// BEGIN Spec part
			".rept 4;" // accesslen
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
			: "S" (in), "D" (out)
			: "rax", "rbx", "r11", "memory"
		);
	}
}*/

// 78 % success on 4 outputs for case in = not out (weirdly, the false hits all appear at the beginning in a row, 100 100 89 89), 49 % success on 5 outputs for same case
static void fNOTN(void* out_1, void* out_2/*, void* out_3, void* out_4*/, void* in){
	__asm__ volatile(
		"lea rbx, [fNOTN_2];"
		"call fNOTN_1;"
		"xor rax, rax;"
		// BEGIN delay ops 
		".rept 4;" // deplen
		"mov rax, [rsp+rax];"
		"and rax, 0x0;"
		".endr;"
		// BEGIN Spec part
		"mov rbx, [%1+rax];" // prob leads to some interleaving/parallel processing which is desired
		"mov rbx, [%2+rax];"
		//"mov rbx, [%3+rax];"
		//"mov rbx, [%4+rax];"
		"lfence;"
		// END Spec part
		"fNOTN_1: mov [rsp], rbx;" 
		"mov rbx, [rsi];" // load input
		"add [rsp], rbx;" // data dependency between input and ptr adrs
		"ret;"
		
		"fNOTN_2: nop;"
		: 
		: "S" (in), "r" (out_1), "r" (out_2)//, "r" (out_3), "r" (out_4)
		: "rax", "rbx", "memory"
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

// 4 outputs 51 % correct on a nand b: 
static void fNANDN(void *in1, void *in2, void *out1, void *out2/*, void *out3, void *out4*/){
	__asm__ volatile(
        "call fNANDN_1;"
		// BEGIN spec code
        "xor rax, rax;"
		".rept 35;"
        "mov rax, [rsp+rax];"
        "and rax, 0x0;"
		".endr;"
		"mov r11, [%2+rax];" // out
		"mov r11, [%3+rax];" // out
		//"mov r11, [%4+rax];" // out
		//"mov r11, [%5+rax];" // out
		// END spec code
        "lfence;"
        "fNANDN_1: lea r11, [fNANDN_2];"
		"mov [rsp], r11;" // move 
        "mov r11, [%0];" // in1
        "add r11, [%1];" // in2
        "add [rsp], r11;"
        "ret;"
        "fNANDN_2: nop;"
        : 
        : "r" (in1), "r" (in2), "r" (out1), "r" (out2)//, "r" (out3)//, "r" (out4)
        : "rax", "r11", "memory"
    );
}

// 4 outputs 51 % correct on A nor notB, 2 outputs 100 %
static void fNORN(void *in1, void *in2, void *out1, void *out2/*, void *out3, void *out4*/){
	__asm__ volatile(
		"lea rbx, [fNORN_3];"
		"call fNORN_1;"
		"call fNORN_2;"
		// BEGIN spec part 
		"xor rax, rax;"
		// BEGIN delay ops 
		".rept 5;"
		"mov rax, [rsp+rax];"
		"and rax, 0x0;"	
		".endr;"		
		// END delay ops 
		"mov r11, [%2+rax];" // addr output + 0
		"mov r11, [%3+rax];" // addr output + 0
		//"mov r11, [%4+rax];" // addr output + 0
		//"mov r11, [%5+rax];" // addr output + 0
		"lfence;"
		// END spec part 
		"fNORN_1: mov [rsp], rbx;"		// in2
		"mov r11, [%1];"
		"add [rsp], r11;"
		"ret;"
		
		"fNORN_2: mov [rsp], rbx;"		// in1
		"mov r11, [%0];"
		"add [rsp], r11;"
		"ret;"
		// end 
		"fNORN_3: nop;"
		: 
		: "r" (in1), "r" (in2), "r" (out1), "r" (out2)//, "r" (out3), "r" (out4)
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

static void fXOR(void *out, void *in1, void *in2, void **buf){

	fNOTN(in1, buf[0], buf[1]); // 6 dump
	fNOTN(in2, buf[2], buf[3]); // 

	printf("after nota %lu %lu\n", probe(buf[0]), probe(buf[1]));

	//!(!A & !B)
	fNAND(buf[4], buf[0], buf[2]);
	printf("after nand %lu\n", probe(buf[4]));
	// !A | !B
	fOR(buf[5], buf[1], buf[3]);	
	printf("after or %lu\n", probe(buf[5]));

	//!(!A & !B) & (!A | !B) <=> (A | B) & !(A & B) <=> A & !B | !A & B
	fAND(out, buf[4], buf[5]);
	
}









