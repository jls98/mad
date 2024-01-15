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

uint32_t load_eight_adrs(void *p0)
{
	volatile unsigned long time;    
    __asm__ volatile (
        " mfence            \n"
        " lfence            \n"
        " rdtsc             \n"
        " mov rcx, rax 		\n"
        " mov rax, [%1]		\n"
        " mov rax, [%1+0x7000]\n"
        " mov rax, [%1+0x3000]\n"
        " mov rax, [%1+0x5000]\n"
        " mov rax, [%1+0x4000]\n"
        " mov rax, [%1+0x1000]\n"
        " mov rax, [%1+0x2000]\n"
        " mov rax, [%1+0x6000]\n"
        " lfence            \n"
        " rdtsc             \n"
        " sub rax, rcx 		\n"
        : "=&a" (time)
        : "r" (p0)
        : "rdx", "rcx");
    return time;
}

uint32_t load_nince_adrs(void *p0)
{
	volatile unsigned long time;    
    __asm__ volatile (
        " mfence            \n"
        " lfence            \n"
        " rdtsc             \n"
        " mov rcx, rax 		\n"
        " mov rax, [%1]		\n"
		" add rax, rcx		\n"
        " mov rax, [%1+0x7000]\n"
		" add rax, rbx		\n"
        " mov rax, [%1+0x3000]\n"
		" add rax, rcx\n"
        " mov rax, [%1+0x5000]\n"
		" add rax, rax\n"
        " mov rax, [%1+0x4000]\n"
    	" add rax, rbx\n"
		" mov rax, [%1+0x1000]\n"
		" add rax, rbx\n"
        " mov rax, [%1+0x2000]\n"
		" add rax, rbx\n"
        " mov rax, [%1+0x6000]\n"
		" add rax, rbx\n"
        " mov rax, [%1+0x8000]\n"
        " lfence            \n"
        " rdtsc             \n"
        " sub rax, rcx 		\n"
        : "=&a" (time)
        : "r" (p0)
        : "rdx", "rcx");
    return time;
}

uint32_t flush_reload(void *p)
{
	volatile unsigned long time;    
    __asm__ volatile (
        " mfence            \n"
        " lfence            \n"
        " rdtsc             \n"
        " mov r8, rax 		\n"
        " mov rax, [%1+0x8000]		\n"
        " lfence            \n"
        " rdtsc             \n"
        " sub rax, r8 		\n"
        : "=&a" (time)
        : "r" (p)
        : "rdx", "r8");
    return time;
}

#define THRESHOLD 80000
#define BUFFER 5
void do_something(){
	// access 9 different locations
	char *filename = "/home/jia/workshop5/zero_file.txt";

	
	void *p0 = map(filename, 0);
	//void *p8 = map(filename, 8*offset);
	uint32_t probe_times[BUFFER] = {0};

	
	probe_times[0] = load_eight_adrs(p0);
	probe_times[1] = load_eight_adrs(p0);
	flush_reload(p0);
	probe_times[2]=load_eight_adrs(p0);
	probe_times[3]=load_nince_adrs(p0);
	probe_times[4]=load_nince_adrs(p0);
	/*for(int i=0;i<BUFFER-1;)
	{
		if (probe_times[i] > THRESHOLD) i++;
		probe_times[i] = 	load_eight_adrs(p0);

	}
	load_eight_adrs(p0);
	probe_times[BUFFER-1] = load_eight_adrs(p0);

	*/
	for(int i=0;i<BUFFER;i++) printf("%u\n", probe_times[i]);
//	printf("sum of probe times of uncached is: %u\nsum of probe times of cached is: %u\n", probe_times[0], probe_times[1]);
}
#define ACCELERATOR 1000000000

int main(){
	uint64_t start = rdtsc();
	while(rdtsc()-start < ACCELERATOR); //
	do_something();
	return 0;
}