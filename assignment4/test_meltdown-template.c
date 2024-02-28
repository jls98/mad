#include "meltdown-template.c"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>



int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 4", NULL, NULL);
   

	CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}