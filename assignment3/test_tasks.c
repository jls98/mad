#include "tasks.c"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

// broken now :S
void test_fNOT(){
	// preparation
	uint64_t *a = malloc(sizeof(uint64_t *));
	uint64_t *b = malloc(sizeof(uint64_t *));
	uint64_t *c = malloc(sizeof(uint64_t *));
	uint64_t *d = malloc(sizeof(uint64_t *));
	uint64_t *e = malloc(sizeof(uint64_t *));
	*a=0;
	*b=0;
	wait(1E9);
	uint64_t time; 
	uint64_t ctr_notA=0, ctr_A=0;
	
	// ------------ not A ------------
	flush(a);
	flush(b);
	fence();
	fNOT(b, a);
	fence();
	time = probe(b);	
	fence();
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT case not A: time is %lu\n", time);
	free(a);
	free(b);
	a = malloc(sizeof(uint64_t *));
	b = malloc(sizeof(uint64_t *));
	*a=0;

	// ------------ A ------------
	uint64_t *a2 = malloc(sizeof(uint64_t *));
	uint64_t *b2 = malloc(sizeof(uint64_t *));
	*a2 = 0;
	fence();
	flush(b2);
	load(a2);

	fence();
	fNOT(b2, a2);
	time = probe(b);
	
	fence();
	CU_ASSERT_TRUE(time>THRESHOLD);
	printf("fNOT case A: time is %lu\n", time);
	
	fence();
	// ---- not2
	flush(a);
	flush(b);
	flush(c);
	
	fNOT2(c, b, a);
	time = probe(b);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT2 case notA: b is %lu\n", time);	
	
	time = probe(c);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT2 case notA: c is %lu\n", time);

	// ------------ A ------------

	flush(b);
	flush(c);
	load(a);

	fNOT2(c, b, a);
	time = probe(b);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT2 case A: b is %lu\n", time);	
	
	time = probe(c);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT2 case A: c is %lu\n", time);
	
	
	// ---- not3
	flush(a);
	flush(b);
	flush(c);
	flush(d);
	
	fNOT3(d, c, b, a);
	time = probe(b);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT3 case notA: b is %lu\n", time);	
	
	time = probe(c);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT3 case notA: c is %lu\n", time);
	
	time = probe(d);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT3 case notA: d is %lu\n", time);

	// ------------ A ------------

	flush(b);
	flush(c);
	flush(d);
	load(a);

	fNOT3(d, c, b, a);
	time = probe(b);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT3 case A: b is %lu\n", time);	
	
	time = probe(c);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT3 case A: c is %lu\n", time);
	
	time = probe(d);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT3 case A: d is %lu\n", time);
	
	
	// ---- not4
	flush(a);
	flush(b);
	flush(c);
	flush(d);
	flush(e);
	
	fNOT4(e, d, c, b, a);
	time = probe(b);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT4 case notA: b is %lu\n", time);	
	
	time = probe(c);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT4 case notA: c is %lu\n", time);
	
	time = probe(d);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT4 case notA: d is %lu\n", time);
	
	time = probe(e);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT4 case notA: e is %lu\n", time);

	// ------------ A ------------

	flush(b);
	flush(c);
	flush(d);
	flush(e);
	load(a);

	fNOT4(e, d, c, b, a);
	time = probe(b);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT4 case A: b is %lu\n", time);	
	
	time = probe(c);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT4 case A: c is %lu\n", time);
	
	time = probe(d);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT4 case A: d is %lu\n", time);
	
	time = probe(e);	
	CU_ASSERT_TRUE(time<THRESHOLD);
	printf("fNOT4 case A: e is %lu\n", time);
	
	// -----
	/*// not A multiple times // TOdo naive version does not work since training effect
	for (int i=0;i<100;i++){
		// not A
		flush(a);
		flush(b);
		
		fNOT(b, a);
		time = probe(b);
		if(time<THRESHOLD) ctr_notA++;
		
		// A
		flush(b);
		load(a);
		
		fNOT(b, a);
		time = probe(b);
		if(time>THRESHOLD) ctr_A++;
	}	
	CU_ASSERT_TRUE(ctr_notA>90);
	if(ctr_notA<=90) printf("ctr_notA %lu\n", ctr_notA);
	CU_ASSERT_TRUE(ctr_A>90);
	if(ctr_A<=90) printf("ctr_A %lu\n", ctr_A);*/
	free(a);
	free(b);
	free(c);
	free(d);
	free(e);
}

void test_fNAND(){
	// preparation

	wait(1E9);
	uint64_t time;
	

	// notA nand notB = C
	uint64_t *a = malloc(sizeof(uint64_t *));
	uint64_t *b = malloc(sizeof(uint64_t *));
	uint64_t *c = malloc(sizeof(uint64_t *));
	*a=0;
	*b=0;

	flush(a);
	flush(b);
	flush(c);
	
	fNAND(c, a, b);
	fence();
	time = probe(c);
	fence();
	CU_ASSERT_TRUE(time<THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("\n\nfNAND case notA, notB: c %lu\n", time);
	
	free(a);
	free(b);
	free(c);
	
	
	// notA nand B = C 
	uint64_t *a1 = malloc(sizeof(uint64_t *));
	uint64_t *b1 = malloc(sizeof(uint64_t *));
	uint64_t *c1 = malloc(sizeof(uint64_t *));
	*a1=0;
	*b1=0;

	flush(a1);
	flush(c1);
	load(b1);
	
	fNAND(c1, a1, b1);
	time = probe(c1);
	CU_ASSERT_TRUE(time>THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("fNAND case notA, B: c %lu\n", time);
	
	free(a1);
	free(b1);
	free(c1);
	
	
	// A nand notB = C 
	uint64_t *a2 = malloc(sizeof(uint64_t *));
	uint64_t *b2 = malloc(sizeof(uint64_t *));
	uint64_t *c2 = malloc(sizeof(uint64_t *));
	*a2=0;
	*b2=0;

	flush(b2);
	flush(c2);
	load(a2);
	
	fNAND(c2, a2, b2);
	time = probe(c2);
	CU_ASSERT_TRUE(time>THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("fNAND case A, notB: c %lu\n", time);
	
	free(a2);
	free(b2);
	free(c2);
	
	/*// A nand B = notC 
	flush((void *) c);
	load((void *) a);
	load((void *) b);
		
	fNAND((void *) c, (void *) a, (void *) b);
	time = probe((void *) c);
	CU_ASSERT_TRUE(time>THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("fNAND case A, B: time is %lu\n", time);

	free(a);
	free(b);
	free(c);*/
}

void test_fNOR(){
	// preparation
	uint64_t *a = malloc(sizeof(uint64_t *));
	uint64_t *b = malloc(sizeof(uint64_t *));
	uint64_t *c = malloc(sizeof(uint64_t *));
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

	free(a);
	free(b);
}


int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 3", NULL, NULL);
    //CU_add_test(suite, "Test fNOT", test_fNOT);
    CU_add_test(suite, "Test fNAND", test_fNAND);
    //CU_add_test(suite, "Test fNOT", test_fNOR);

    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}