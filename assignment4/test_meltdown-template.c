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
    cc_init();
    CU_ASSERT_TRUE(((u64 *)cc_buffer)[0] != 0xaaaaaaaaaaaaaaaa);
    cc_setup();
    u64 time;
    void *cur_adrs;
    for (int i=0; i<cc_buf_size/4096;i++){
        cur_adrs=&cc_buffer[i*512];
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
    for(int i=0; i<256;i++){
        cc_setup();
        cc_transmit(i);
        uint64_t time = cc_receive();
        CU_ASSERT_EQUAL(time, i);
        if (time !=i){
            printf("transmission failed at %i\n", i);
        }
    }
}


int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 4", NULL, NULL);
   	CU_add_test(suite, "test_cc_init", test_cc_init);
   	CU_add_test(suite, "test_cc_setup", test_cc_setup);
   	CU_add_test(suite, "test_cc_transmission", test_cc_transmission);


	CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}