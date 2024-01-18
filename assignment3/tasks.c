#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <x86intrin.h>

#define THRESHOLD 160 // timing around 14-16 when cached

static void fNOT(void *out, void *in); // NOT gate
static void fNOT2(void* out_1, void* out_2, void* in);
static void fNOTN(void* out_1, void* out_2, void* out_3, void* out_4,void* out_5, void* out_6,void* out_7, void* out_8,void* out_9,void* out_10,void* out_11, void* in);

static void fNORN(void *in1, void *in2, void *out1, void *out2, void *out3, void *out4, void *out5, void *out6, void *out7, void *out8, void *out9, void *out10, void *out11, void *out12);

//static void fNOTX(void *out, void *in, uint64_t x); // xNOT gate with x out
static void fNOR(void *out, void *in1, void *in2);
static void fNAND(void *out, void *in1, void *in2);
static void fNANDN(void *in1, void *in2, void *out1, void *out2, void *out3, void *out4, void *out5, void *out6, void *out7, void *out8, void *out9, void *out10, void *out11, void *out12);
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
		"lea rbx, [rip+fNOT_2];"
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

static void fNOT2(void* out_1, void* out_2, void* in){
	__asm__ volatile(
		"lea rbx, fNOT2_2[rip];"
		"call fNOT2_1;"
		"xor rax, rax;"
		// BEGIN delay ops 
		".rept 4;" // deplen
		"mov rax, [rsp+rax];"
		"and rax, 0x0;"
		".endr;"
		// BEGIN Spec part
		"mov rbx, [%1+rax];" // prob leads to some interleaving/parallel processing which is desired
		"mov rbx, [%2+rax];"
		"lfence;"
		// END Spec part
		"fNOT2_1: mov [rsp], rbx;" 
		"mov rbx, [rsi];" // load input
		"add [rsp], rbx;" // data dependency between input and ptr adrs
		"ret;"
		
		"fNOT2_2: nop;"
		: 
		: "S" (in), "r" (out_1), "r" (out_2)
		: "rax", "rbx", "memory"
	);
}

static void fNOTN(void* out_1, void* out_2, void* out_3, void* out_4,void* out_5, void* out_6,void* out_7, void* out_8,void* out_9,void* out_10,void* out_11, void* in){
	__asm__ volatile(
		"lea rbx, fNOTN_2[rip];"
		"call fNOTN_1;"
		"xor rax, rax;"
		// BEGIN delay ops 
		".rept 30;" // deplen
		"mov rax, [rsp+rax];"
		"and rax, 0x0;"
		".endr;"
		// BEGIN Spec part
		"mov r11, [%1+rax];" // prob leads to some interleaving/parallel processing which is desired
		"mov r11, [%2+rax];"
		"mov r11, [%3+rax];"
		"mov r11, [%4+rax];"
		"lfence;"
		// END Spec part
		"fNOTN_1: mov [rsp], rbx;" 
		"mov rbx, [rsi];" // load input
		"add [rsp], rbx;" // data dependency between input and ptr adrs
		"ret;"
		
		"fNOTN_2: nop;"
		: 
		: "S" (in), "r" (out_1), "r" (out_2), "r" (out_3), "r" (out_4)
		: "rax", "r11", "rbx", "memory"
	);
}

static void fNOR(void *out, void *in1, void *in2){
	__asm__ volatile(
		"lea rbx, [rip+fNOR_3];"
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
		"lea rbx, [rip+fNAND_2];"
        "call fNAND_1;"
		// BEGIN spec code
        "xor rax, rax;"
		".rept 10;"
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
static void fNANDN(void *in1, void *in2, void *out1, void *out2, void *out3, void *out4, void *out5, void *out6, void *out7, void *out8, void *out9, void *out10, void *out11, void *out12){
	__asm__ volatile(
        "call fNANDN_1;"
		// BEGIN spec code
        "xor rax, rax;"
		".rept 4;"
        "mov rax, [rsp+rax];"
        "and rax, 0x0;"
		".endr;"
		"mov r11, [%2+rax];" // out1
		"mov r11, [%3+rax];" // out2
		"mov r11, [%4+rax];" // out3
		"mov r11, [%5+rax];" // out4
		"mov r11, [%6+rax];" // out5
		"mov r11, [%7+rax];" // out6
		"mov r11, [%8+rax];" // out7
		"mov r11, [%9+rax];" // out8
		"mov r11, [%10+rax];" // out9
		"mov r11, [%11+rax];" // out10
	//	"mov r11, [%12+rax];" // out
		// END spec code
        "lfence;"
        "fNANDN_1: lea r11, fNANDN_2[rip];"
		"mov [rsp], r11;" // move 
        "mov r11, [%0];" // in1
        "add r11, [%1];" // in2
        "add [rsp], r11;"
        "ret;"
        "fNANDN_2: nop;"
        : 
        : "r" (in1), "r" (in2), "r" (out1), "r" (out2), "r" (out3), "r" (out4), "r" (out5), "r" (out6), "r" (out7), "r" (out8), "r" (out9), "r" (out10)
        : "rax", "r11", "memory"
    );
}

// 4 outputs 51 % correct on A nor notB, 2 outputs 100 %
static void fNORN(void *in1, void *in2, void *out1, void *out2, void *out3, void *out4, void *out5, void *out6, void *out7, void *out8, void *out9, void *out10, void *out11, void *out12){
	__asm__ volatile(
		"call fNORN_1;"
		"call fNORN_2;"
		// BEGIN spec part 
		"xor rax, rax;"
		// BEGIN delay ops 
		".rept 10;"
		"mov rax, [rsp+rax];"
		"and rax, 0x0;"	
		".endr;"		
		// END delay ops 
		"mov r11, [%2+rax];" // out1
		"mov r11, [%3+rax];" // out2
		"mov r11, [%4+rax];" // out3
		"mov r11, [%5+rax];" // out4
		"mov r11, [%6+rax];" // out5
		"mov r11, [%7+rax];" // out6
		"lfence;"
		// END spec part 
		"fNORN_1: lea r11, fNORN_3[rip];"
        "mov [rsp], r11;"		// in2
		"mov r11, [%1];"
		"add [rsp], r11;"
		"ret;"
		
		"fNORN_2: lea r11, fNORN_3[rip];"
        "mov [rsp], r11;"		// in1
		"mov r11, [%0];"
		"add [rsp], r11;"
		"ret;"
		// end 
		"fNORN_3: nop;"
		: 
		: "r" (in1), "r" (in2), "r" (out1), "r" (out2), "r" (out3), "r" (out4), "r" (out5), "r" (out6)//, "r" (out7), "r" (out8), "r" (out9), "r" (out10), "r" (out11), "r" (out12)
		: "rax", "rbx", "memory"
	);
}

static void fOR(void *out, void *in1, void *in2){
	__asm__ volatile(
		"lea rbx, [rip+fOR_2];"
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
        
		"fOR_2: lea rbx, [rip+fOR_4];"
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
		"lea rbx, [rip+fAND_2];"
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

	fNOT2(buf[0], buf[1], in1); // 6 dump
	fNOT2(buf[2], buf[3], in2); // 

	//printf("b4 nand 0 %lu 2 %lu\n", probe(buf[0]), probe(buf[2]));

	fence();
	//!(!A & !B)
	fNAND(buf[4], buf[0], buf[2]);
	//printf("after nand %lu\n", probe(buf[4]));
	// !A | !B
	fOR(buf[5], buf[1], buf[3]);	
	//printf("after or %lu\n", probe(buf[5]));
	fence();

	//!(!A & !B) & (!A | !B) <=> (A | B) & !(A & B) <=> A & !B | !A & B
	fAND(out, buf[4], buf[5]);
	
}

// out1: !A*D + B*C + !A*!C + B*!D + !B*!C*D + A*C*!D

static void fLED1(void *in1, void *in2, void *in3, void *in4, void *out, void **buf){
    
    
    // !A: 1, 4, A: 3 
    fNOT2(buf[0], buf[1], in1); // !
    fNOT2(buf[2], buf[3], buf[0]);
    fNOT(buf[4], buf[2]); // !
    
    flush(buf[0]);
    flush(buf[2]);
    
    // !b: 6, b: 7, 8
    fNOT2(buf[5], buf[6], in2); //!
    fNOT2(buf[7], buf[8], buf[5]);

    flush(buf[5]);

    // !c: 10, 13, c: 12
    fNOT2(buf[9], buf[10], in3); //!
    fNOT2(buf[11], buf[12], buf[9]);
    fNOT(buf[13], buf[11]); //!

    flush(buf[9]);
    flush(buf[11]);

    // !d: 2, 11, d: 9
    fNOT2(buf[0], buf[2], in4); //!
    fNOT2(buf[5], buf[9], buf[0]);
    fNOT(buf[11], buf[5]);   //!

    flush(buf[0]);
    flush(buf[5]);
    
}

static void fLED2(void *in1, void *in2, void *in3, void *in4, void *out){
    
}

static void fLED3(void *in1, void *in2, void *in3, void *in4, void *out){
    
}

static void fLED4(void *in1, void *in2, void *in3, void *in4, void *out){
    
}

static void fLED5(void *in1, void *in2, void *in3, void *in4, void *out){
    
}

static void fLED6(void *in1, void *in2, void *in3, void *in4, void *out){
    
}

static void fLED7(void *in1, void *in2, void *in3, void *in4, void *out){
    
}

// according to the naming convention of the first gates, I started counting from 1 and not from 0, so in1 is in0 from the task and out1 is out0 from the task respectively
static void fLED(void *in1, void *in2, void *in3, void *in4, void *out1, void *out2, void *out3, void *out4, void *out5, void *out6, void *out7, void **buf){
    // A 3, 7, 10, 11, 12, 13, 14,
    fNOT2(buf[0], buf[1], in1); // !
	__asm__ volatile("lfence");
    fNOT2(buf[2], buf[3], buf[0]);
	__asm__ volatile("lfence");
    fNOT2(buf[4], buf[5], buf[2]); // !
	__asm__ volatile("lfence");
    fNOT2(buf[6], buf[7], buf[4]);
	__asm__ volatile("lfence");
    fNOT2(buf[8], buf[9], buf[6]); // !
	__asm__ volatile("lfence");
    fNOT2(buf[10], buf[11], buf[8]);
	__asm__ volatile("lfence");
    
    fNOT(buf[12], buf[1]);
	__asm__ volatile("lfence");
    fNOT(buf[13], buf[5]);
	__asm__ volatile("lfence");
    fNOT(buf[14], buf[9]);
	__asm__ volatile("lfence");
    
    flush(buf[0]);
    flush(buf[1]);
    flush(buf[2]);
    flush(buf[4]);
    flush(buf[5]);
    flush(buf[6]);
    flush(buf[8]);
    flush(buf[9]);
    
   /* printf("a dup is %lu\n", probe(buf[3]));
    printf("a dup is %lu\n", probe(buf[7]));
    printf("a dup is %lu\n", probe(buf[10]));
    printf("a dup is %lu\n", probe(buf[11]));
    printf("a dup is %lu\n", probe(buf[12]));
    printf("a dup is %lu\n", probe(buf[13]));
    printf("a dup is %lu\n", probe(buf[14]));*/
    
    // >14
    // B 4, 9, 17, 18, 19, 20, 21
	__asm__ volatile("mfence");
    fNOT2(buf[0], buf[1], in2); // !
	__asm__ volatile("lfence");
    fNOT2(buf[2], buf[4], buf[0]);
	__asm__ volatile("lfence");
    fNOT2(buf[5], buf[6], buf[2]); // !
	__asm__ volatile("lfence");
    fNOT2(buf[8], buf[9], buf[5]);
	__asm__ volatile("lfence");
    fNOT2(buf[15], buf[16], buf[8]); // !
	__asm__ volatile("lfence");
    fNOT2(buf[17], buf[18], buf[15]);
	__asm__ volatile("lfence");
    
    fNOT(buf[19], buf[1]);
	__asm__ volatile("lfence");
    fNOT(buf[20], buf[6]);
	__asm__ volatile("lfence");
    fNOT(buf[21], buf[16]);
	__asm__ volatile("lfence");
    
    flush(buf[0]);
    flush(buf[1]);
    flush(buf[2]);
    flush(buf[5]);
    flush(buf[6]);
    flush(buf[8]);
    flush(buf[15]);
    flush(buf[16]);

    /*printf("b dup is %lu\n", probe(buf[4]));
    printf("b dup is %lu\n", probe(buf[9]));
    printf("b dup is %lu\n", probe(buf[17]));
    printf("b dup is %lu\n", probe(buf[18]));
    printf("b dup is %lu\n", probe(buf[19]));
    printf("b dup is %lu\n", probe(buf[20]));
    printf("b dup is %lu\n", probe(buf[21]));*/
    
    // > 21
    // C 5, 16, 24, 25, 26, 27, 28
	__asm__ volatile("mfence");
    fNOT2(buf[0], buf[1], in3); // !
	__asm__ volatile("lfence");
    fNOT2(buf[2], buf[5], buf[0]);
	__asm__ volatile("lfence");
    fNOT2(buf[6], buf[8], buf[2]); // !
	__asm__ volatile("lfence");
    fNOT2(buf[15], buf[16], buf[6]);
	__asm__ volatile("lfence");
    fNOT2(buf[22], buf[23], buf[15]); // !
	__asm__ volatile("lfence");
    fNOT2(buf[24], buf[25], buf[22]);
	__asm__ volatile("lfence");
    
    fNOT(buf[26], buf[1]);
	__asm__ volatile("lfence");
    fNOT(buf[27], buf[8]);
	__asm__ volatile("lfence");
    fNOT(buf[28], buf[23]);
	__asm__ volatile("lfence");
    
    flush(buf[0]);
    flush(buf[1]);
    flush(buf[2]);
    flush(buf[6]);
    flush(buf[8]);
    flush(buf[15]);
    flush(buf[22]);
    flush(buf[23]);
   /* 
    printf("c dup is %lu\n", probe(buf[5]));
    printf("c dup is %lu\n", probe(buf[16]));
    printf("c dup is %lu\n", probe(buf[24]));
    printf("c dup is %lu\n", probe(buf[25]));
    printf("c dup is %lu\n", probe(buf[26]));
    printf("c dup is %lu\n", probe(buf[27]));
    printf("c dup is %lu\n", probe(buf[28]));*/

    
    // > 28
    // D 0, 1, 2, 6, 8, 15, 23
	__asm__ volatile("mfence");
    fNOT2(buf[0], buf[1], in4); // !
	__asm__ volatile("lfence");
    fNOT2(buf[2], buf[6], buf[0]);
	__asm__ volatile("lfence");
    flush(buf[0]);
	__asm__ volatile("lfence");
    fNOT2(buf[0], buf[8], buf[2]); // !
	__asm__ volatile("mfence");
    flush(buf[2]);
	__asm__ volatile("mfence");
    fNOT2(buf[2], buf[15], buf[0]);
	__asm__ volatile("mfence");
    flush(buf[0]);
	__asm__ volatile("mfence");
    fNOT2(buf[0], buf[22], buf[2]); // !
	__asm__ volatile("mfence");
    flush(buf[2]);
	__asm__ volatile("mfence");
    fNOT2(buf[2], buf[23], buf[0]);
	__asm__ volatile("mfence");
    flush(buf[0]);
	__asm__ volatile("mfence");
    fNOT(buf[0], buf[1]);
	__asm__ volatile("mfence");
    flush(buf[1]);
	__asm__ volatile("mfence");
    fNOT(buf[1], buf[8]);
	__asm__ volatile("mfence");
    flush(buf[8]);
	__asm__ volatile("mfence");
    fNOT(buf[8], buf[22]);
	__asm__ volatile("mfence");
/*
    printf("d dup is %lu\n", probe(buf[0]));
    printf("d dup is %lu\n", probe(buf[1]));
    printf("d dup is %lu\n", probe(buf[2]));
    printf("d dup is %lu\n", probe(buf[7]));
    printf("d dup is %lu\n", probe(buf[8]));
    printf("d dup is %lu\n", probe(buf[15]));
    printf("d dup is %lu\n", probe(buf[23]));*/

    void **buff = buf+29*sizeof(void *);
    fLED1(buf[3], buf[4], buf[5], buf[0], out1);
    fLED2(buf[7], buf[9], buf[16], buf[1], out2);
    fLED3(buf[10], buf[17], buf[24], buf[2], out3);
    fLED4(buf[11], buf[18], buf[25], buf[6], out4);
    fLED5(buf[12], buf[19], buf[26], buf[8], out5);
    fLED6(buf[13], buf[20], buf[27], buf[15], out6);
    fLED7(buf[14], buf[21], buf[28], buf[23], out7);
}







