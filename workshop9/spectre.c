#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define THRESHOLD 170
// init covert channel
void cc_init();

// set channel for sending byte
void cc_setup();

// transmit a value through channel
void cc_transmit(uint8_t value);

// return received value or -1 if error
uint8_t cc_receive();

// clflush adrs p
void flush(void *p);
// load adrs adrs
void load(void *adrs);

uint64_t probe(void *adrs);

void *cc;

void cc_init(){
	
    cc = malloc(256*sizeof(void *)); // allocate space for 256 cache lines
}

void cc_setup(){
    for(int i=0;i<256;i++) flush(&cc[i]); // flush cc  
}

void cc_transmit(uint8_t value){
    
    __asm__ volatile (
        "mov rax,%0;"
        "mov rbx,0x8;"
        "mul rax;"
        "mov rbx, [%1];"
        "add rbx, rax;"
        "mov rax, [rbx];"
    
        ::"r" (value), "r" (cc): "rax", "rbx");
}

uint8_t cc_receive(){
    uint8_t value=0;
    for(int i=0;i<256;i++){
        if (probe(&cc[i]) > THRESHOLD){
            return i;
        }
    }
    return -1;
}

void flush(void *p)
{
    __asm__ volatile ("clflush [%0]":: "r" (p));
}

void load(void *adrs){
	__asm__ volatile("mov rax, [%0];"::"r" (adrs): "rax", "memory");
}

uint64_t probe(void *adrs){
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
        "clflush [%1];"
        : "=&a" (time)
        : "r" (adrs)
        : "ecx", "rdx", "r8", "memory"
	);
	return time;
}

int main(){
    cc_init();
    cc_setup();
    cc_transmit(1);
    printf("received %u\n", cc_receive());
}






