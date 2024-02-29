#include "meltdown-template.c"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_cc_init(){
    CU_ASSERT_TRUE(cc_buffer == NULL);
    cc_init();
    CU_ASSERT_TRUE(cc_buffer != NULL);
    munmap(cc_buffer, cc_buf_size);
}

void test_cc_setup(){
    // wait(1E9);
    cc_init();
    CU_ASSERT_TRUE(((u64 *)cc_buffer)[cc_buf_offset] != 0xaaaaaaaaaaaaaaaa);
    cc_setup();
    u64 time, start, end;
    void *cur_adrs;
    for (int i=0; i< (int)256;i++){
        cur_adrs=&cc_buffer[cc_buf_offset+i*512];
        start = my_rdtsc();
        maccess(cur_adrs);
        end = my_rdtsc();
        my_mfence();
        CU_ASSERT_TRUE(*((u64 *)cur_adrs) == 0xaaaaaaaaaaaaaaaa);
        my_mfence();
        CU_ASSERT_TRUE((end-start) > threshold);
        flush(cur_adrs);
    }
    
    munmap(cc_buffer, cc_buf_size);
}

void test_cc_transmission(){
    // wait(1E9);
    cc_init();
    cc_setup();
    for(int i=0; i<256;i++){
        my_mfence();
        cc_transmit(i); 
        my_mfence();        
        //consistently fails to transmit 247 - 255, sometimes unreliable (reason?)
        int time = cc_receive();
        CU_ASSERT_EQUAL(time, i);
        if (time !=i){
           printf("transmission failed at %i\n", i);
        }
    }
    
    munmap(cc_buffer, cc_buf_size);
}

void test_meltdown(){
    // test valid values
    // wait(1E9);
    cc_init();
    uint8_t test_num;
    int received;
    for (int i=0;i<256;i++){
        my_mfence(); 
        test_num=i;
        my_mfence(); 
        meltdown((uintptr_t) &test_num);
        CU_ASSERT_EQUAL(test_num, cc_receive());
        my_mfence(); 
        received = do_meltdown((uintptr_t) &test_num);
        my_mfence(); 
        CU_ASSERT_EQUAL(test_num, received);
        my_mfence(); 
        if (test_num != received) printf("Test fail: test_num %u, received %i\n", test_num, received);
        my_mfence(); 
        received = do_meltdown((uintptr_t) &test_num);
        my_mfence(); 
        CU_ASSERT_EQUAL(test_num, received);
        my_mfence(); 
        if (test_num != received) printf("Test fail: test_num %u, received %i\n", test_num, received);
        my_mfence();
    }
    
    // measuring something outside of allocated mem space
    // test segfault
    uintptr_t target = (uintptr_t) &cc_buffer[12*283000];
    CU_ASSERT_FALSE(do_meltdown( target)==-1);
    CU_ASSERT_FALSE(do_meltdown( target)==-1);
    CU_ASSERT_FALSE(do_meltdown( target)==-1);
    
    
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
    CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), 65);
    CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), 65);
    CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), 65);
    
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
    
    // does this work?
    // test segfault
    CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), -1);
    CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), -1);
    CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), -1);    

    munmap(cc_buffer, cc_buf_size);
}


int main() {
    wait(1E9);
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 4", NULL, NULL);
   	CU_add_test(suite, "test_cc_init", test_cc_init);
   	CU_add_test(suite, "test_cc_setup", test_cc_setup);
   	CU_add_test(suite, "test_cc_transmission", test_cc_transmission);
   	CU_add_test(suite, "test_meltdown", test_meltdown);


	CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}