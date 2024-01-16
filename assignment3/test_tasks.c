#include "tasks.c"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

// broken now :S
void test_fNOT(){
	// preparation
	uint64_t *a = (uint64_t *) malloc(sizeof(uint64_t *));
	uint64_t *b = (uint64_t *) malloc(sizeof(uint64_t *));
	*a=0;
	*b=0;
	wait(1E9);
	uint64_t time; //, ctr_notA=0, ctr_A=0;
	
	// ------------ not A ------------
	flush(a);
	flush(b);
	
	fNOT(b, a);
	time = probe(b);
	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT case not A: time is %lu\n", time);

	// ------------ A ------------

	flush(b);
	load(a);

	fNOT(b, a);
	time = probe(b);
	
	CU_ASSERT_TRUE(time>THRESHOLD);
	printf("fNOT case A: time is %lu\n", time);
	
	// not A multiple times // TOdo naive version does not work since training effect
	/*for (int i=0;i<100;i++){
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
		
		fibonacci(10);
	}	
	CU_ASSERT_TRUE(ctr_notA>90);
	if(ctr_notA<=90) printf("ctr_notA %lu\n", ctr_notA);
	CU_ASSERT_TRUE(ctr_A>90);
	if(ctr_A<=90) printf("ctr_A %lu\n", ctr_A);*/
}
/*
void test_fNAND(){
	// preparation
	uint64_t *a = (uint64_t *) malloc(sizeof(uint64_t *));
	uint64_t *b = (uint64_t *) malloc(sizeof(uint64_t *));
	uint64_t *c = (uint64_t *) malloc(sizeof(uint64_t *));
	*a=0;
	*b=0;
	*c=0;
	wait(1E9);
	uint64_t time;
	
	// notA nand notB = C
	flush(a);
	flush(b);
	flush(c);
	
	fNAND(c, a, b);
	time = probe(c);
	CU_ASSERT_TRUE(time<THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("\n\nfNAND case notA, notB: time is %lu\n", time);
	
	// notA nand B = C 
	flush(a);
	flush(b);
	flush(c);
	load(b);
	
	fNAND(c, a, b);
	time = probe(c);
	CU_ASSERT_TRUE(time<THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("fNAND case notA, B: time is %lu\n", time);
	
	// A nand notB = C 
	flush(a);
	flush(b);
	flush(c);
	load(a);
	
	fNAND(c, a, b);
	time = probe(c);
	CU_ASSERT_TRUE(time<THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("fNAND case A, notB: time is %lu\n", time);
	
	// A nand B = notC 
	flush(a);
	flush(b);
	flush(c);
	load(a);
	load(b);
	
	// test 
	printf("testing a %lu\n",probe(a));
	printf("testing b %lu\n",probe(b));
	
	fNAND(c, a, b);
	time = probe(c);
	CU_ASSERT_TRUE(time>THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("fNAND case A, B: time is %lu\n", time);
	
}

void test_fNOR(){
	// preparation
	uint64_t *a = (uint64_t *) malloc(sizeof(uint64_t *));
	uint64_t *b = (uint64_t *) malloc(sizeof(uint64_t *));
	uint64_t *c = (uint64_t *) malloc(sizeof(uint64_t *));
	*a=0;
	*b=0;
	*c=0;
	wait(1E9);
	uint64_t time;
	
	// notA nor notB = C
	flush(a);
	flush(b);
	flush(c);
	
	fNOR(c, a, b);
	time = probe(c);
	CU_ASSERT_TRUE(time<THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("\n\nfNOR case notA, notB: time is %lu\n", time);
	
	// notA nor B = notC 
	flush(a);
	flush(b);
	flush(c);
	load(b);
	
	fNOR(c, a, b);
	time = probe(c);
	CU_ASSERT_TRUE(time>THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("fNOR case notA, B: time is %lu\n", time);
	
	// A nor notB = notC 
	flush(a);
	flush(b);
	flush(c);
	load(a);
	
	fNOR(c, a, b);
	time = probe(c);
	CU_ASSERT_TRUE(time>THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("fNOR case A, notB: time is %lu\n", time);
	
	// A nor B = notC 
	flush(a);
	flush(b);
	flush(c);
	load(a);
	load(b);
	
	fNOR(c, a, b);
	time = probe(c);
	CU_ASSERT_TRUE(time>THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("fNOR case A, B: time is %lu\n", time);
}

*/
int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 3", NULL, NULL);
    CU_add_test(suite, "Test fNOT", test_fNOT);
  //  CU_add_test(suite, "Test fNOT", test_fNAND);
    //CU_add_test(suite, "Test fNOT", test_fNOR);

    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}