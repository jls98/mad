#include "tasks.c"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_fNOT(){
	// preparation
	uint64_t *a = (uint64_t *) malloc(sizeof(uint64_t *));
	uint64_t *b = (uint64_t *) malloc(sizeof(uint64_t *));
	*a=0;
	*b=0;
	wait(1E9);
	uint64_t time, ctr_notA=0, ctr_A=0;
	
	
	flush(a);
	flush(b);
	// not A
	fNOT(b, a);
	time = probe(b);
	CU_ASSERT_TRUE(time<THRESHOLD);
	if(time>=THRESHOLD) printf("time>THRESHOLD: time is %lu\n", time);

	
	flush(a);
	flush(b);
	load(a);

	// A
	fNOT(b, a);
	time = probe(b);
	CU_ASSERT_TRUE(time>THRESHOLD);
	if(time<=THRESHOLD) printf("time<THRESHOLD: time is %lu\n", time);

	// not A multiple times
	for (int i=0;i<10000;i++){
		flush(a);
		flush(b);

		// not A
		fNOT(b, a);
		time = probe(b);
		if(time<THRESHOLD) ctr_notA++;
		
		flush(a);
		flush(b);
		load(a);

		// A
		fNOT(b, a);
		time = probe(b);
		if(time>THRESHOLD) ctr_A++;
	}	
	CU_ASSERT_TRUE(ctr_notA>9000);
	if(ctr_notA<=9000) printf("ctr_notA %lu\n", ctr_notA);
	CU_ASSERT_TRUE(ctr_A>9000);
	if(ctr_A<=9000) printf("ctr_A %lu\n", ctr_A);
}




int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 3", NULL, NULL);
    CU_add_test(suite, "Test fNOT", test_fNOT);

    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}