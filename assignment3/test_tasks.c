#include "tasks.c"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

// broken now :S
void test_fNOT(){
	// preparation
	uint64_t time; 
	wait(1E9);

	// ------------ not A ------------
	for(int i=0;i<10000;i++){
		void *mm = malloc(8192);
		void *in=mm;
		void *out = mm+4096+64; // +page size +cache line
		
		*((uint64_t *)in) =0;
		
		flush(in);
		flush(out);
		
		fence();
		fNOT(out, in);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time<THRESHOLD);
		//printf("fNOT case not A: time is %lu\n", time);
		free(mm);
	}
	
	// ------------ A ------------
	for(int i=0;i<10000;i++){
		void *mm = malloc(8192);
		void *in=mm;
		void *out = mm+4096+64; // +page size +cache line

		*((uint64_t *)in) =0;
		flush(out);
		load(in);
		
		fence();
		fNOT(out, in);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		//printf("fNOT case not A: time is %lu\n", time);
		free(mm);
	}
	
}

void test_fNAND(){
	wait(1E9);
	uint64_t time;
	
	// notA nand notB = C
	for(int i=0;i<10000;i++){
		void *mm = malloc(12240);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;
		
		flush(in1);
		flush(in2);
		flush(out);
		
		fence();
		fNAND(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// notA nand B = C 
	for(int i=0;i<10000;i++){
		void *mm = malloc(12240);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;
		
		flush(in1);
		load(in2);
		flush(out);
		
		fence();
		fNAND(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// A nand notB = C 
	for(int i=0;i<10000;i++){
		void *mm = malloc(12240);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;
		
		load(in1);
		flush(in2);
		flush(out);
		
		fence();
		fNAND(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// A nand B = notC 
	for(int i=0;i<10000;i++){
		void *mm = malloc(12240);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;
		
		load(in1);
		load(in2);
		flush(out);
		
		fence();
		fNAND(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	// preparation


	/*

	// notA nand notB = C
	uint64_t *a = malloc(sizeof(uint64_t *));
	uint64_t *b = malloc(sizeof(uint64_t *));
	uint64_t *c = malloc(sizeof(uint64_t *));
	*a=0;
	*b=0;

	flush(a);
	flush(b);
	flush(c);
	
	fence();
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
	
	fence();
	fNAND(c1, a1, b1);
	fence();
	time = probe(c1);
	fence();
	CU_ASSERT_TRUE(time>THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("fNAND case notA, B: c %lu\n", time);
	
	free(a1);
	free(b1);
	free(c1);
	
	
	// A nand B = notC 
	uint64_t *a3 = malloc(sizeof(uint64_t *));
	uint64_t *b3 = malloc(sizeof(uint64_t *));
	uint64_t *c3 = malloc(sizeof(uint64_t *));
	*a3=0;
	*b3=0;

	flush(c3);
	load(a3);
	load(b3);

	fence();
	fNAND(c3, a3, b3);
	fence();
	time = probe(c3);
	fence();
	CU_ASSERT_TRUE(time>THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("fNAND case A, B: c %lu\n", time);
	
	free(a3);
	free(b3);
	free(c3);
	
	
	// A nand notB = C 
	uint64_t *a2 = malloc(sizeof(uint64_t *));
	uint64_t *b2 = malloc(sizeof(uint64_t *));
	uint64_t *c2 = malloc(sizeof(uint64_t *));
	*a2=0;
	*b2=0;

	flush(b2);
	flush(c2);
	load(a2);
	
	fence();
	fNAND(c2, a2, b2);
	fence();
	time = probe(c2);
	fence();

	CU_ASSERT_TRUE(time>THRESHOLD);
	//if(time>=THRESHOLD) 
	printf("fNAND case A, notB: c %lu\n", time);
	
	free(a2);
	free(b2);
	free(c2);*/
}

void test_fNOR(){
	
	wait(1E9);
	uint64_t time;
	
	// notA nor notB = C
	for(int i=0;i<10000;i++){
		void *mm = malloc(12240);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;
		
		flush(in1);
		flush(in2);
		flush(out);
		
		fence();
		fNOR(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// notA nor B = notC 
	for(int i=0;i<10000;i++){
		void *mm = malloc(12240);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;
		
		flush(in1);
		load(in2);
		flush(out);
		
		fence();
		fNOR(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
	// A nor notB = notC 
	for(int i=0;i<10000;i++){
		void *mm = malloc(12240);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;
		
		load(in1);
		flush(in2);
		flush(out);
		
		fence();
		fNOR(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
	// A nor B = notC 
	for(int i=0;i<10000;i++){
		void *mm = malloc(12240);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;
		
		load(in1);
		load(in2);
		flush(out);
		
		fence();
		fNOR(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}

}


int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 3", NULL, NULL);
    CU_add_test(suite, "Test fNOT", test_fNOT);
    CU_add_test(suite, "Test fNAND", test_fNAND);
    CU_add_test(suite, "Test fNOT", test_fNOR);

    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}