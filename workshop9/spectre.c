#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
    return cc;
}

void cc_setup(){
    for(int i=0;i<256;i++) flush(&cc[i]); // flush cc  
    return cc;
}

void cc_transmit(uint8_t value){
    // make val 256 bit binary 
    uint8_t val_cur=value;
    uint8_t remainder =0;
    uint64_t counter = 0;
    
    for(uint64_t counter=0; counter<256 && val_cur>0;counter++){
        // divide by two
        // if remainder = 1
        remainder = val_cur%2;
        val_cur/=2;
        if(remainder){
            load(&cc[counter]);
        }
    }
}

uint8_t cc_receive(){
    
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
}






