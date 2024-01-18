#include "tasks.c"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#define CYC 100

void test_fNOTN(){
	uint64_t time; 
	wait(1E9);
	// x=1
	// ------------ not A ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in=mm;
		void *out1 = mm+4096+64; // +page size +cache line
		void *out2 = mm+2*(4096+64); // +page size +cache line
		//void *out3 = mm+3*(4096+64); // +page size +cache line
		//void *out4 = mm+4*(4096+64); // +page size +cache line
		
		*((uint64_t *)in) =0;
		
		flush(in);
		flush(out1);
		flush(out2);
		//flush(out3);
		//flush(out4);
		
		fence();
		fNOTN(out1, out2/*, out3, out4*/, in);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		//time = probe(out3);	
		//CU_ASSERT_TRUE(time<THRESHOLD);
		//time = probe(out4);	
		//CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// ------------ not A ------------
		for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in=mm;
		void *out1 = mm+4096+64; // +page size +cache line
		void *out2 = mm+2*(4096+64); // +page size +cache line
		//void *out3 = mm+3*(4096+64); // +page size +cache line
		//void *out4 = mm+4*(4096+64); // +page size +cache line
		
		*((uint64_t *)in) =0;
		
		flush(out1);
		flush(out2);
		//flush(out3);
		//flush(out4);
		load(in);
		
		fence();
		fNOTN(out1, out2/*, out3, out4*/, in);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		//time = probe(out3);	
		//CU_ASSERT_TRUE(time>THRESHOLD);
		//time = probe(out4);	
		//CU_ASSERT_TRUE(time>THRESHOLD);
		//printf("fNOT case not A: time is %lu\n", time);
		free(mm);
	}
}

void test_fNANDN(){
	uint64_t time; 
	wait(1E9);
	// ------------ notA nand notB ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+3*(4096+64); // +page size +cache line
		//void *out3 = mm+4*(4096+64); // +page size +cache line
		//void *out4 = mm+5*(4096+64); // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(in1);
		flush(in2);
		flush(out1);
		flush(out2);
		//flush(out3);
		//flush(out4);
		
		fence();
		fNANDN(in1, in2, out1, out2/*, out3, out4*/);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		//time = probe(out3);	
		//CU_ASSERT_TRUE(time<THRESHOLD);
		//time = probe(out4);	
		//CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// ------------ A nand notB ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+3*(4096+64); // +page size +cache line
		//void *out3 = mm+4*(4096+64); // +page size +cache line
		//void *out4 = mm+5*(4096+64); // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(in2);
		flush(out1);
		flush(out2);
		//flush(out3);
		//flush(out4);
		load(in1);
		
		fence();
		fNANDN(in1, in2, out1, out2/*, out3, out4*/);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		//time = probe(out3);	
		//CU_ASSERT_TRUE(time<THRESHOLD);
		//time = probe(out4);	
		//CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
		// ------------ notA nand B ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+3*(4096+64); // +page size +cache line
		//void *out3 = mm+4*(4096+64); // +page size +cache line
		//void *out4 = mm+5*(4096+64); // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(in1);
		flush(out1);
		flush(out2);
		//flush(out3);
		//flush(out4);
		load(in2);
		
		fence();
		fNANDN(in1, in2, out1, out2/*, out3, out4*/);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		//time = probe(out3);	
		//CU_ASSERT_TRUE(time<THRESHOLD);
		//time = probe(out4);	
		//CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
		// ------------ A nand B ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+3*(4096+64); // +page size +cache line
		//void *out3 = mm+4*(4096+64); // +page size +cache line
		//void *out4 = mm+5*(4096+64); // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(out1);
		flush(out2);
		//flush(out3);
		//flush(out4);
		load(in1);
		load(in2);
		
		fence();
		fNANDN(in1, in2, out1, out2/*, out3, out4*/);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		//time = probe(out3);	
		//CU_ASSERT_TRUE(time>THRESHOLD);
		//time = probe(out4);	
		//CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
}

void test_fNORN(){
	uint64_t time; 
	wait(1E9);
	// ------------ notA nor notB ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+3*(4096+64); // +page size +cache line
		//void *out3 = mm+4*(4096+64); // +page size +cache line
		//void *out4 = mm+5*(4096+64); // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(in1);
		flush(in2);
		flush(out1);
		flush(out2);
		//flush(out3);
		//flush(out4);
		
		fence();
		fNORN(in1, in2, out1, out2/*, out3, out4*/);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		/*time = probe(out3);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out4);	
		CU_ASSERT_TRUE(time<THRESHOLD);*/
		free(mm);
	}
	
	// ------------ A nor notB ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+3*(4096+64); // +page size +cache line
		//void *out3 = mm+4*(4096+64); // +page size +cache line
		//void *out4 = mm+5*(4096+64); // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(in2);
		flush(out1);
		flush(out2);
		//flush(out3);
		//flush(out4);
		load(in1);
		
		fence();
		fNORN(in1, in2, out1, out2/*, out3, out4*/);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		//time = probe(out3);	
		//CU_ASSERT_TRUE(time>THRESHOLD);
		//time = probe(out4);	
		//CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
		// ------------ notA nor B ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+3*(4096+64); // +page size +cache line
		//void *out3 = mm+4*(4096+64); // +page size +cache line
		//void *out4 = mm+5*(4096+64); // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(in1);
		flush(out1);
		flush(out2);
		//flush(out3);
		//flush(out4);
		load(in2);
		
		fence();
		fNORN(in1, in2, out1, out2/*, out3, out4*/);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		//time = probe(out3);	
		//CU_ASSERT_TRUE(time>THRESHOLD);
		//time = probe(out4);	
		//CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
		// ------------ A nor B ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+3*(4096+64); // +page size +cache line
		//void *out3 = mm+4*(4096+64); // +page size +cache line
		//void *out4 = mm+5*(4096+64); // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(out1);
		flush(out2);
		//flush(out3);
		//flush(out4);
		load(in1);
		load(in2);
		
		fence();
		fNORN(in1, in2, out1, out2/*, out3, out4*/);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		//time = probe(out3);	
		//CU_ASSERT_TRUE(time>THRESHOLD);
		//time = probe(out4);	
		//CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
}
	
void test_fNOT(){
	// preparation
	uint64_t time; 
	wait(1E9);

	// ------------ not A ------------
	for(int i=0;i<CYC;i++){
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
	for(int i=0;i<CYC;i++){
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
	for(int i=0;i<CYC;i++){
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
	for(int i=0;i<CYC;i++){
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
	for(int i=0;i<CYC;i++){
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
	for(int i=0;i<CYC;i++){
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
	for(int i=0;i<CYC;i++){
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
		//printf("fNOR case notA, notB: C is %lu\n", time);
		
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// notA nor B = notC 
	for(int i=0;i<CYC;i++){
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
		//printf("fNOR case notA, B: C is %lu\n", time);
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
	// A nor notB = notC 
	for(int i=0;i<CYC;i++){
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
		//printf("fNOR case A, notB: C is %lu\n", time);
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
	// A nor B = notC 
	for(int i=0;i<CYC;i++){
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
		//printf("fNOR case A, B: C is %lu\n", time);
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
}


void test_fAND(){
	wait(1E9);
	uint64_t time;
	
	// notA and notB = notC
	for(int i=0;i<CYC;i++){
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
		fAND(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
	// notA and B = notC 
	for(int i=0;i<CYC;i++){
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
		fAND(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
	// A and notB = notC 
	for(int i=0;i<CYC;i++){
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
		fAND(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
	// A and B = C 
	for(int i=0;i<CYC;i++){
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
		fAND(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		//printf("fand case A B: C is %lu\n", time);
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
}

void test_fOR(){
	wait(1E9);
	uint64_t time;
	
	// notA and notB = notC
	for(int i=0;i<CYC;i++){
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
		fOR(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
	// notA and B = notC 
	for(int i=0;i<CYC;i++){
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
		fOR(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// A and notB = notC 
	for(int i=0;i<CYC;i++){
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
		fOR(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// A and B = C 
	for(int i=0;i<CYC;i++){
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
		fOR(out, in1, in2);
		fence();
		time = probe(out);	
		fence();
		
		//printf("fand case A B: C is %lu\n", time);
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
}

void test_fXOR(){
	wait(1E9);
	uint64_t time;
	
	// notA xor notB = notC
	for(int i=0;i<CYC;i++){
		void *mm = malloc(81920);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		void **buf = malloc(7*sizeof(void *));
		for(int j=0;j<7;j++){
			buf[j]=mm+(3+j)*(4096+64);
			*((uint64_t *)buf[j]) = 0;
			flush(buf[j]);
		}		
		flush(in1);
		flush(in2);
		flush(out);
		
		fence();
		fXOR(out, in1, in2, buf);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
	// notA and B = C 
	for(int i=0;i<CYC;i++){
		void *mm = malloc(81920);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		void **buf = malloc(7*sizeof(void *));
		for(int j=0;j<7;j++){
			buf[j]=mm+(3+j)*(4096+64);
			*((uint64_t *)buf[j]) = 0;
			flush(buf[j]);
		}		
		flush(in1);
		flush(out);
		load(in2);
		
		fence();
		fXOR(out, in1, in2, buf);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// A and notB = C 
	for(int i=0;i<CYC;i++){
		void *mm = malloc(81920);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		void **buf = malloc(7*sizeof(void *));
		for(int j=0;j<7;j++){
			buf[j]=mm+(3+j)*(4096+64);
			*((uint64_t *)buf[j]) = 0;
			flush(buf[j]);
		}		
		flush(in2);
		flush(out);
		load(in1);
		
		fence();
		fXOR(out, in1, in2, buf);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}

	
	// A and B = notC 
	for(int i=0;i<CYC;i++){
		void *mm = malloc(81920);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		void **buf = malloc(7*sizeof(void *));
		for(int j=0;j<7;j++){
			buf[j]=mm+(3+j)*(4096+64);
			*((uint64_t *)buf[j]) = 0;
			flush(buf[j]);
		}		
		load(in1);
		flush(out);
		load(in2);
		
		fence();
		fXOR(out, in1, in2, buf);
		fence();
		time = probe(out);	
		fence();
		
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
}

void test(){
		void *mm = malloc(81920);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out = mm+8192+128; // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		void **buf = malloc(7*sizeof(void *));
		for(int j=0;j<7;j++){
			buf[j]=mm+(3+j)*(4096+64);
			*((uint64_t *)buf[j]) = 0;
			flush(buf[j]);
		}		
		flush(out);
		load(in1);
		load(in2);
		
		fence();
		fXOR(out, in1, in2, buf);
		fence();
		uint64_t time = probe(out);	
		fence();
		
		//CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
}

int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 3", NULL, NULL);
    int normal = 0;
	
	if(normal){
		CU_add_test(suite, "Test fNOT", test_fNOT);
		CU_add_test(suite, "Test fNOTN", test_fNOTN);
		CU_add_test(suite, "Test fNAND", test_fNAND); 
		CU_add_test(suite, "Test fAND", test_fAND); 
		CU_add_test(suite, "Test fNOR", test_fNOR);
		CU_add_test(suite, "Test fOR", test_fNOR);
	}
	//CU_add_test(suite, "Test fNANDN", test_fNANDN); // TODO
    //CU_add_test(suite, "Test fNORN", test_fNORN); // TODO
    //CU_add_test(suite, "Test fXOR", test_fXOR); // TODO
    test();
	CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}

