#include "meltdown-template.c"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_cc_init(){
    cc_init();
    CU_ASSERT_TRUE(cc_buffer != NULL);
}


int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 4", NULL, NULL);
   	CU_add_test(suite, "test_cc_init", test_cc_init);


	CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}