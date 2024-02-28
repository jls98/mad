#include "meltdown-template.c"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_cc_init(){
    CU_ASSERT_TRUE(cc_buffer == NULL);
    cc_init();
    CU_ASSERT_TRUE(cc_buffer != NULL);
    munmap(cc_buffer, cc_buf_size);
    CU_ASSERT_TRUE(cc_buffer == NULL);
}

void test_cc_setup(){
    cc_init();
    CU_ASSERT_TRUE(((char *)cc_buffer)[0] != 0xaa);
    cc_setup();
    u64 time;
    for (int i=0; i<cc_buf_size/4096;i++){
        cur_adrs=&cc_buffer[i*512];
        time = my_rdtsc();
        maccess(cur_adrs);
        time = my_rdtsc() - time;
        
        CU_ASSERT_TRUE(((char *)&cc_buffer[i*512])[0] == 0xaa);
        CU_ASSERT_TRUE(time > threshold);
        flush(cur_adrs);
    }
}


int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 4", NULL, NULL);
   	CU_add_test(suite, "test_cc_init", test_cc_init);


	CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}