#include "meltdown-template.c"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#define CC_REPS 100

void test_cc_init(){
    CU_ASSERT_TRUE(cc_buffer == NULL);
    cc_init();
    CU_ASSERT_TRUE(cc_buffer != NULL);
    munmap(cc_buffer, cc_buf_size);
}

void test_cc_setup(){
    cc_init();
    CU_ASSERT_TRUE(((u64 *)cc_buffer)[cc_buf_offset] != 0xaaaaaaaaaaaaaaaa);
    cc_setup();
    u64 time;
    void *cur_adrs;
    for (int i=0; i< (int)256;i++){
        cur_adrs=&cc_buffer[cc_buf_offset+i*512];
        time = my_rdtsc();
        maccess(cur_adrs);
        time = my_rdtsc() - time;
        CU_ASSERT_TRUE(*((u64 *)cur_adrs) == 0xaaaaaaaaaaaaaaaa);
        CU_ASSERT_TRUE(time > threshold);
        flush(cur_adrs);
    }
}

void test_cc_transmission(){
    wait(1E9);
    cc_setup();
    int miss_cnt=0;
    for(int i=0; i<256;i++){
        my_mfence();
        for(int j=0;j<CC_REPS;j++){ // repeat 1000 times to evlauate reliability of covert channel
            my_mfence();
            cc_transmit(i); 
            my_mfence();        
            int time = cc_receive();
            CU_ASSERT_EQUAL(time, i);
            if (time !=i){
               printf("transmission failed at %i\n", i);
               miss_cnt++;
            }
        }
        my_mfence();
    }
    printf("miss_cnt is %i\n", miss_cnt);
}

void test_meltdown(){
    // test valid values
    wait(1E9);
    cc_init();
    uint8_t test_num;
    for (int i=0;i<256;i++){
        test_num=i;
        meltdown((uintptr_t) &test_num);
        CU_ASSERT_EQUAL(test_num, cc_receive());
        my_mfence(); 
        CU_ASSERT_EQUAL(test_num, do_meltdown((uintptr_t) &test_num));
        my_mfence(); 
        CU_ASSERT_EQUAL(test_num, do_meltdown((uintptr_t) &test_num));
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
    
    // does this work? -> receiving/reading nothing
    // test segfault
    // CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), 65);
    // CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), 65);
    // CU_ASSERT_EQUAL(do_meltdown((uintptr_t) file_ptr), 65);    

    munmap(cc_buffer, cc_buf_size);
}


int main() {
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