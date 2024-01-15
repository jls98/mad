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

/*
Write the function void fr_trace(void *p1, void *p2, uint32_t period, uint32_t count, uint32_t *trace).
The function uses Flush+Reload to check access to both p1 and p2 every period cycles. It repeats the 
checks count times, storing the results in trace. Specifically, upon return, the first iteration's 
reload times of p1 and p2 are stored in trace[0] and trace[1], respectively. More generally, the results 
of the ith iteration are stored in trace[2*i] and trace[2*i+1]. You can assume that the array trace is 
large enough to store all the results.

Use the function to build a binary fr_trace <file> <offset1> <offset2> that traces accesses to offset1 
and offset2 in the file file. Use a period of 10000 cycles and collect 1000 such samples. The output is 
1000 lines, each with two numbers -- the probe times of offset1 and offset2. 

To test fr_test, use our rattle program /home/victim/rattle. rattle uses the same arguments as fr_trace, 
and generates a repeated access pattern.  To use, run rattle in the background, then run fr_trace with 
the same arguments, and submit the output trace as trace.out.  Additionally, plot the collected trace 
and include the resulting figure in the answers PDF. See below for an example of such a trace.
*/
void fr_trace(void *p1, void *p2, uint32_t period, uint32_t count, uint32_t *trace)
{
	uint32_t start; 
	int volatile a;
	for(uint32_t i=0;i<count;i++) // repeat count times
	{
		start = rdtsc();
		trace[2*i] = flush_reload(p1); // probe p1
		trace[2*i+1] = flush_reload(p2); // probe p1
		while(rdtsc() - start < period) a++; // wait period 
	}
}

#define COUNT 1000
#define PERIOD 10000

int main(int ac, char **av){
	// input validation
	if (ac != 4) return 1;
	char *filename = av[1];
	printf("filename %s\n", filename);
	uint64_t offset1 = atoi(av[2]);
	printf("offset1 %li\n", offset1);
	uint64_t offset2 = atoi(av[3]);
	printf("offset2 %li\n", offset2);
	
	uint32_t a = 0;
	
	
	uint64_t start = rdtsc();
	while(rdtsc()-start < 1000000000) a++; // 
	uint32_t traces[2*COUNT] = {0};
	
	void *p1 = map(filename, offset1);
	void *p2 = map(filename, offset2);
	
	
	fr_trace(p1, p2, PERIOD, COUNT, traces);
	
	printf("p1---------------------\n");
	for (int i=0;i< COUNT;i++)printf("%u\n",traces[2*i]);
	
	printf("\n\np2---------------------\n");
	for (int i=0;i< COUNT;i++)printf("%u\n",traces[2*i+1]);
		
	
	return 0; // finish programm, unmap mapping
}
