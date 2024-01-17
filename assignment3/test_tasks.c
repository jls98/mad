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
}

void test_fNOR(){
	
	wait(1E9);
	uint64_t time;
	
	// notA nor notB = C
	for(int i=0;i<10;i++){
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
		printf("fNOR case notA, notB: C is %lu\n", time);
		
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// notA nor B = notC 
	for(int i=0;i<10;i++){
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
		printf("fNOR case notA, B: C is %lu\n", time);
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
	// A nor notB = notC 
	for(int i=0;i<10;i++){
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
		printf("fNOR case A, notB: C is %lu\n", time);
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
	// A nor B = notC 
	for(int i=0;i<10;i++){
		void *mm = malloc(2*8192);
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
		printf("fNOR case A, B: C is %lu\n", time);
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
}


int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 3", NULL, NULL);
    CU_add_test(suite, "Test fNOT", test_fNOT);
    CU_add_test(suite, "Test fNAND", test_fNAND);
    CU_add_test(suite, "Test fNOR", test_fNOR);

    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}