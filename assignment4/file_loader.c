#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
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
    
    int cnt=0;
    while(1){
        cnt++;
    }
    // Close the file descriptor since it's no longer needed
    close(fd);

   
    printf("cnt %i\n", cnt);
    // Unmap the file from memory
    if (munmap(file_ptr, stat_buf.st_size) == -1) {
        perror("Error unmapping file from memory");
        exit(EXIT_FAILURE);
    }

    return 0;
}
