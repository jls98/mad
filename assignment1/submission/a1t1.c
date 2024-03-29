#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*
Write the function void *map(char *file_name, uint64_t offset). The function uses the mmap to map a single page of the file file_name into the 
address space of the process. The page to map is the one that contains offset offset in the file. The function returns a pointer to the memory 
address in which offset is mapped. That is, reading from the memory location reads the contents of the specified offset in the file. Mapping 
should be private (MAP_PRIVATE) and read only (PROT_READ). In the case that the function detects an error, it should return NULL. However, 
you are not required to test for all possible errors. In particular, you can assume that offset is valid and does not point beyond the end of 
the file.
*/

#define DEFAULT_FILENAME "zero_file.txt"
#define DEFAULT_OFFSET 0

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

int main(int ac, char **av){
	char *filename = DEFAULT_FILENAME;
	uint64_t offset = DEFAULT_OFFSET;
	if(ac==3){
		filename=av[1];
		offset = atoi(av[2]);
	}
	
	void *location = map(filename, offset);
	printf("mapping is located at %p\n", location);
	return 0;
}
