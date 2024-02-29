#include "meltdown-template.c"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_cc_setup(){
    // value not written to adrs yet
    CU_ASSERT_TRUE(((u64 *)cc_buffer)[cc_buf_offset] != 0xaaaaaaaaaaaaaaaa);
    
    u64 time, start, end;
    void *cur_adrs;
    for (int i=0; i< (int)256;i++){
        cc_setup();
        my_mfence();
        cur_adrs=&cc_buffer[cc_buf_offset+i*offset];
        start = my_rdtsc();
        maccess(cur_adrs);
        end = my_rdtsc();
        my_mfence();
        // a written all over the place, yay
        CU_ASSERT_TRUE(*((u64 *)cur_adrs) == 0xaaaaaaaaaaaaaaaa);
        time = end-start;
        CU_ASSERT_TRUE(time > threshold);
        if (time <= threshold) printf("Test fail: diff is %lu at %i\n", time, i);
        my_mfence();
        flush(cur_adrs);
    }
}

void test_cc_transmission(){
   
    cc_setup();
    for(int i=0; i<256;i++){
        my_mfence();
        cc_transmit(i); 
        my_mfence();        
        int time = cc_receive();
        CU_ASSERT_EQUAL(time, i);
        if (time !=i){
           printf("transmission failed at %i with received val %i\n", i, time);
        }
    }
    
}

void test_meltdown(){
    // test valid values
    uint8_t *test_num = malloc(sizeof(uint8_t));
    int received;
    for (int i=0;i<256;i++){
        my_mfence(); 
        *test_num=i;
        my_mfence(); 
        meltdown((uintptr_t) test_num);
        my_mfence(); 
        received = cc_receive();
        my_mfence();
        CU_ASSERT_EQUAL(*test_num, received);
        if (*test_num != received) printf("Test fail: test_num %u, received %i\n", *test_num, received);
        my_mfence(); 
        received = do_meltdown((uintptr_t) test_num);
        my_mfence(); 
        CU_ASSERT_EQUAL(*test_num, received);
        if (*test_num != received) printf("Test fail: test_num %u, received %i\n", *test_num, received);
        my_mfence(); 
        received = do_meltdown((uintptr_t) test_num);
        my_mfence(); 
        CU_ASSERT_EQUAL(*test_num, received);
        if (*test_num != received) printf("Test fail: test_num %u, received %i\n", *test_num, received);
        my_mfence();
    }
    
    // measuring something outside of allocated mem space
    // test segfault
    uintptr_t target = (uintptr_t) &cc_buffer[12*283000];
    my_mfence();
    received = do_meltdown(target);
    my_mfence(); 
    CU_ASSERT_FALSE(received==-1);
    if (received==-1) printf("Test fail: received %i, should not be -1\n", received);
    
    my_mfence();
    received = do_meltdown(target);
    my_mfence(); 
    CU_ASSERT_FALSE(received==-1);
    if (received==-1) printf("Test fail: received %i, should not be -1\n", received);
    
    my_mfence();
    received = do_meltdown(target);
    my_mfence(); 
    CU_ASSERT_FALSE(received==-1);
    if (received==-1) printf("Test fail: received %i, should not be -1\n", received);
    my_mfence();
     
    
    // test via a file
    const char *filename = "./file"; // contains only "AA..."
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
    
    my_mfence();
    received = do_meltdown((uintptr_t) file_ptr);
    my_mfence(); 
    CU_ASSERT_EQUAL(received, 65);
    if (received!=65) printf("Test fail: received %i, should be 65\n", received);
    
    my_mfence();
    received = do_meltdown((uintptr_t) file_ptr);
    my_mfence(); 
    CU_ASSERT_EQUAL(received, 65);
    if (received!=65) printf("Test fail: received %i, should be 65\n", received);
    
    my_mfence();
    received = do_meltdown((uintptr_t) file_ptr);
    my_mfence(); 
    CU_ASSERT_EQUAL(received, 65);
    if (received!=65) printf("Test fail: received %i, should be 65\n", received);
    my_mfence();
    
    // Close the file descriptor since it's no longer needed
    close(fd);

    CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), 65);
    CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), 65);
    CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), 65);
    
    // Unmap the file from memory
    if (munmap(file_ptr, stat_buf.st_size) == -1) {
        perror("Error unmapping file from memory");
        exit(EXIT_FAILURE);
    }
    
    // test segfault
    CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), -1);
    CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), -1);
    CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), -1);    

}


int main() {
    wait(1E9);
    cc_init();
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 4", NULL, NULL);
   	// CU_add_test(suite, "test_cc_init", test_cc_init);
   	CU_add_test(suite, "test_cc_setup", test_cc_setup);
   	CU_add_test(suite, "test_cc_transmission", test_cc_transmission);
   	CU_add_test(suite, "test_meltdown", test_meltdown);

	CU_basic_run_tests();
    CU_cleanup_registry();

    munmap(cc_buffer, cc_buf_size);
    return 0;
}