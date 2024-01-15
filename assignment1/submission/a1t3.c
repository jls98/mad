#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*
Write the function int monitor(void *p, uint32_t period, uint64_t maxwait). The function 
uses Flush+Reload to monitor the location p every period cycles. When the function detects 
that p has been accessed, it returns 1. If maxwait cycles pass without p being accessed, 
the function returns 0. While the period is not expected to be fully accurate, you should 
take some measures to reduce variance where possible.
*/

#define ACCELERATOR 1000000000
#define DEFAULT_FILENAME "zero_file.txt"
#define DEFAULT_OFFSET 0
#define PERIOD 10000 // 100000 seems to be a good number
#define MAXWAIT 10000000000

void *map(char *file_name, uint64_t offset)
{
    long page_size = sysconf(_SC_PAGE_SIZE); // 4096
	int file_descriptor = open(file_name, O_RDONLY); 
    if (file_descriptor == -1) return NULL;
    struct stat st_buf;
    if (fstat(file_descriptor, &st_buf) == -1) return NULL;
	uint64_t mmap_offset = offset/page_size; // calculate page pointed to
    size_t map_len = st_buf.st_size - mmap_offset*page_size;
	void *mapping = mmap(NULL, map_len, PROT_READ, MAP_PRIVATE, file_descriptor, mmap_offset*page_size);
	if (mapping == MAP_FAILED){
		printf("mmap fail with errno %d\n", errno); // fix problems with mmap
		return NULL;
	}
	return mapping;
}

static __inline__ uint64_t rdtsc(void)
{
    uint32_t lo, hi;
    __asm__ volatile ("rdtscp" : "=a" (lo), "=d" (hi) : : "ecx");
    return ((uint64_t)hi<<32)|lo;
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
	uint64_t start, global_start = rdtsc();
	while(rdtsc()-global_start < ACCELERATOR);
	global_start = rdtsc();
	while (rdtsc() - global_start < maxwait)
	{	
		start = rdtsc();
		if (flush_reload(p) < 100) return 1; // detected activity -> return 1
		while(rdtsc() - start < period); // wait period
	}
	return 0;
}

int main(int ac, char **av){
	char *filename = DEFAULT_FILENAME;
	uint64_t offset = DEFAULT_OFFSET;
	if(ac==3){
		filename=av[1];
		offset = atoi(av[2]);
	}
	void *location = map(filename, offset);
	printf("monitoring returned %i (0=no detection, 1=detection)\n", monitor(location, PERIOD, MAXWAIT));
	return 0; // finish programm, unmap mapping
}

// very few operations while period time check -> with O0 almost precise (with variable assignment to get values overhead was between 0 and 40 cycles only)