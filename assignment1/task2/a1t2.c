#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
/*
Implement the function uint32_t flush_reload(void *p), which performs the core function of
the Flush+Reload attack. Specifically, the function measures the time to access the memory 
location in p and flushes that memory from the cache.  You need to take care of all 
required fencing. You can assume that the access time fits in 32 bits. That is, no need to 
test for overflow.

Use the function to collect access times to cached and to non-cached memory locations.
*/

#define MEASUREMENT_AMOUNT 100 // set amount of measurements to calculate the statistical values from

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

// Function to swap two elements in an array
void swap(uint32_t *a, uint32_t *b) {
    uint32_t temp = *a;
    *a = *b;
    *b = temp;
}

// Partition the array into two sub-arrays, and return the pivot index
int partition(uint32_t arr[], int low, int high) {
    uint32_t pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// Quicksort algorithm
void quicksort(uint32_t arr[], int low, int high) {
    if (low < high) {
        int pivot = partition(arr, low, high);

        quicksort(arr, low, pivot - 1);
        quicksort(arr, pivot + 1, high);
    }
}

void printUInt32Array(uint32_t arr[], int size) {
    if (size == 0) {
        printf("[]\n");
        return;
    }

    printf("[%i", arr[0]);
    for (int i = 1; i < size; i++) {
        printf(", %i", arr[i]);
    }
    printf("]\n");
}


int main(){
	uint64_t start = rdtsc();
	while(rdtsc()-start < 1000000000);
	void *location = map("zero_file.txt", 4096);

	printf("this is the location of the mapping %p\n", location);
	
	flush_reload(((char *)location)); // flush
	uint32_t measurement1 = flush_reload(((char *)location)); // uncached
	char volatile random = *((char *)location);
	for (int j=0;j<100;j++) random = *((char *)location); // "training" the cache
	
	uint32_t measurement2 = flush_reload(((char *)location)); // cached

	printf("\nmeasurement uncached is %i\nmeasurement cached is %i\nsome location contains %c :D\n", measurement1, measurement2, random);

	// get a couple of measurements
	uint32_t measurements_cached[MEASUREMENT_AMOUNT] = {0};
	uint32_t measurements_uncached[MEASUREMENT_AMOUNT] = {0};
	double cached_avg = 0.0f, uncached_avg = 0.0f;
	for (int i=0; i<MEASUREMENT_AMOUNT; i++)
	{
		measurements_uncached[i] = flush_reload(((char *)location));		
		for (int j=0;j<100;j++) random = *((char *)location); // "training" the cache
		measurements_cached[i] = flush_reload(((char *)location));
		
		// calculate average
		cached_avg+=(((double)measurements_cached[i])/((double)MEASUREMENT_AMOUNT));
		uncached_avg+=(((double)measurements_uncached[i])/((double)MEASUREMENT_AMOUNT));
	}
	
	printf("Average access time of cached is %f cycles\nAverage access time of uncaced is %f cycles\n", cached_avg, uncached_avg);
	
	// calculate median
	quicksort(measurements_cached, 0, MEASUREMENT_AMOUNT - 1);
	quicksort(measurements_uncached, 0, MEASUREMENT_AMOUNT - 1);
	printf("mean of uncached is %i\nmean of cached is %i\n", measurements_uncached[MEASUREMENT_AMOUNT/2], measurements_cached[MEASUREMENT_AMOUNT/2]);
	
	printUInt32Array(measurements_cached, MEASUREMENT_AMOUNT);
	printUInt32Array(measurements_uncached, MEASUREMENT_AMOUNT);
	return 0; // finish programm, unmap mapping
}
