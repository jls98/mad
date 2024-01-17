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



void *cc_init(){
	
    void *cc = malloc(256*sizeof(void *)); // allocate space for 256 cache lines
    return cc;


}

void cc_setup(){
    void *cc = cc_init();
    for(int i=0;i<256;i++){
        flush(&cc[0]);
        printf("%i\n",i);
    }
}


void flush(void *p)
{
    __asm__ volatile ("clflush [%0]":: "r" (p));
}

int main(){
    cc_setup();
}






