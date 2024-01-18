#include "tasks.c"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#define CYC 10

void test_fNOT2(){
	uint64_t time; 
	wait(1E9);
	// x=1
	// ------------ not A ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in=mm;
		void *out1 = mm+4096+64; // +page size +cache line
		void *out2 = mm+2*(4096+64); // +page size +cache line

		*((uint64_t *)in) =0;
		
		flush(in);
		flush(out1);
		flush(out2);
		
		fence();
		fNOT2(out1, out2, in);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// ------------ not A ------------
		for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in=mm;
		void *out1 = mm+4096+64; // +page size +cache line
		void *out2 = mm+2*(4096+64); // +page size +cache line

		*((uint64_t *)in) =0;
		
		flush(out1);
		flush(out2);
		load(in);
		
		fence();
		fNOT2(out1, out2, in);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		//printf("fNOT case not A: time is %lu\n", time);
		free(mm);
	}
}

void test_fNOTN(){
	uint64_t time; 
	wait(1E9);
	// x=1
	// ------------ not A ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(81920);
		void *in=mm;
		void *out1 = mm+4096+64; // +page size +cache line
		void *out2 = mm+2*(4096+64); // +page size +cache line
		void *out3 = mm+3*(4096+64); // +page size +cache line
		void *out4 = mm+4*(4096+64); // +page size +cache line
		void *out5 = mm+5*(4096+64); // +page size +cache line
		void *out6 = mm+6*(4096+64); // +page size +cache line
		void *out7 = mm+7*(4096+64); // +page size +cache line
		void *out8 = mm+8*(4096+64); // +page size +cache line
		void *out9 = mm+9*(4096+64); // +page size +cache line
		void *out10 = mm+10*(4096+64); // +page size +cache line
		void *out11 = mm+11*(4096+64); // +page size +cache line
		void *out12 = mm+12*(4096+64); // +page size +cache line

		*((uint64_t *)in) =0;
		
		flush(in);
		flush(out1);
		flush(out2);
		flush(out3);
		flush(out4);
		flush(out5);
		flush(out6);
		flush(out7);
		flush(out8);
		flush(out9);
		flush(out10);
		flush(out11);
		flush(out12);
		
		fence();
		fNOTN(out1, out2, out3, out4, out5, out6, out7, out8, out9, out10, out11, in);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out3);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out4);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// ------------ not A ------------
		for(int i=0;i<CYC;i++){
		void *mm = malloc(8192);
		void *in=mm;
		void *out1 = mm+4096+64; // +page size +cache line
		void *out2 = mm+2*(4096+64); // +page size +cache line
		void *out3 = mm+3*(4096+64); // +page size +cache line
		void *out4 = mm+4*(4096+64); // +page size +cache line
		void *out5 = mm+5*(4096+64); // +page size +cache line
		void *out6 = mm+6*(4096+64); // +page size +cache line
		void *out7 = mm+7*(4096+64); // +page size +cache line
		void *out8 = mm+8*(4096+64); // +page size +cache line
		void *out9 = mm+9*(4096+64); // +page size +cache line
		void *out10 = mm+10*(4096+64); // +page size +cache line
		void *out11 = mm+11*(4096+64); // +page size +cache line
		void *out12 = mm+12*(4096+64); // +page size +cache line

		*((uint64_t *)in) =0;
		
		flush(out1);
		flush(out2);
		flush(out3);
		flush(out4);
		flush(out5);
		flush(out6);
		flush(out7);
		flush(out8);
		flush(out9);
		flush(out10);
		flush(out11);
		flush(out12);
		load(in);
		
		fence();
		fNOTN(out1, out2, out3, out4, out5, out6, out7, out8, out9, out10, out11, in);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out3);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out4);	
		printf("fNOT case not A: time is %lu\n", time);
		CU_ASSERT_TRUE(time>THRESHOLD);
		printf("fNOT case not A: time is %lu\n", time);
		free(mm);
	}
}

void test_fNANDN(){
	uint64_t time; 
	wait(1E9);
	// ------------ notA nand notB ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(81920);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
        void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+3*(4096+64); // +page size +cache line
        void *out3 = mm+4*(4096+64); // +page size +cache line
		void *out4 = mm+5*(4096+64); // +page size +cache line
		void *out5 = mm+6*(4096+64); // +page size +cache line
		void *out6 = mm+7*(4096+64); // +page size +cache line
		void *out7 = mm+8*(4096+64); // +page size +cache line
		void *out8 = mm+9*(4096+64); // +page size +cache line
		void *out9 = mm+10*(4096+64); // +page size +cache line
		void *out10 = mm+11*(4096+64); // +page size +cache line
		void *out11 = mm+12*(4096+64); // +page size +cache line
		void *out12 = mm+13*(4096+64); // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(in1);
		flush(in2);
		flush(out1);
		flush(out2);
		flush(out3);
		flush(out4);
		flush(out5);
		flush(out6);
		flush(out7);
		flush(out8);
		flush(out9);
		flush(out10);
		flush(out11);
		flush(out12);
		
		fence();
		fNANDN(in1, in2, out1, out2, out3, out4, out5, out6, out7, out8, out9, out10, out11, out12);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out3);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out4);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out5);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out6);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out7);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out8);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out9);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out10);	
		free(mm);
	}
	
	// ------------ A nand notB ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(81920);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
        void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+3*(4096+64); // +page size +cache line
        void *out3 = mm+4*(4096+64); // +page size +cache line
		void *out4 = mm+5*(4096+64); // +page size +cache line
		void *out5 = mm+6*(4096+64); // +page size +cache line
		void *out6 = mm+7*(4096+64); // +page size +cache line
		void *out7 = mm+8*(4096+64); // +page size +cache line
		void *out8 = mm+9*(4096+64); // +page size +cache line
		void *out9 = mm+10*(4096+64); // +page size +cache line
		void *out10 = mm+11*(4096+64); // +page size +cache line
		void *out11 = mm+12*(4096+64); // +page size +cache line
		void *out12 = mm+13*(4096+64); // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		load(in1);
		flush(in2);
		flush(out1);
		flush(out2);
		flush(out3);
		flush(out4);
		flush(out5);
		flush(out6);
		flush(out7);
		flush(out8);
		flush(out9);
		flush(out10);
		flush(out11);
		flush(out12);
		
		fence();
		fNANDN(in1, in2, out1, out2, out3, out4, out5, out6, out7, out8, out9, out10, out11, out12);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out3);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out4);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out5);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out6);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out7);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out8);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out9);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out10);	
		free(mm);
	}
	
		// ------------ notA nand B ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(81920);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
        void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+3*(4096+64); // +page size +cache line
        void *out3 = mm+4*(4096+64); // +page size +cache line
		void *out4 = mm+5*(4096+64); // +page size +cache line
		void *out5 = mm+6*(4096+64); // +page size +cache line
		void *out6 = mm+7*(4096+64); // +page size +cache line
		void *out7 = mm+8*(4096+64); // +page size +cache line
		void *out8 = mm+9*(4096+64); // +page size +cache line
		void *out9 = mm+10*(4096+64); // +page size +cache line
		void *out10 = mm+11*(4096+64); // +page size +cache line
		void *out11 = mm+12*(4096+64); // +page size +cache line
		void *out12 = mm+13*(4096+64); // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(in1);
		load(in2);
		flush(out1);
		flush(out2);
		flush(out3);
		flush(out4);
		flush(out5);
		flush(out6);
		flush(out7);
		flush(out8);
		flush(out9);
		flush(out10);
		flush(out11);
		flush(out12);
		
		fence();
		fNANDN(in1, in2, out1, out2, out3, out4, out5, out6, out7, out8, out9, out10, out11, out12);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out3);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out4);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out5);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out6);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out7);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out8);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out9);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out10);	
		free(mm);
	}
	
		// ------------ A nand B ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(81920);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
        void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+3*(4096+64); // +page size +cache line
        void *out3 = mm+4*(4096+64); // +page size +cache line
		void *out4 = mm+5*(4096+64); // +page size +cache line
		void *out5 = mm+6*(4096+64); // +page size +cache line
		void *out6 = mm+7*(4096+64); // +page size +cache line
		void *out7 = mm+8*(4096+64); // +page size +cache line
		void *out8 = mm+9*(4096+64); // +page size +cache line
		void *out9 = mm+10*(4096+64); // +page size +cache line
		void *out10 = mm+11*(4096+64); // +page size +cache line
		void *out11 = mm+12*(4096+64); // +page size +cache line
		void *out12 = mm+13*(4096+64); // +page size +cache line
		
		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		load(in1);
		load(in2);
		flush(out1);
		flush(out2);
		flush(out3);
		flush(out4);
		flush(out5);
		flush(out6);
		flush(out7);
		flush(out8);
		flush(out9);
		flush(out10);
		flush(out11);
		flush(out12);
		
		fence();
		fNANDN(in1, in2, out1, out2, out3, out4, out5, out6, out7, out8, out9, out10, out11, out12);
		fence();
		time = probe(out1);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out3);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out4);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out5);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out6);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out7);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out8);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out9);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out10);	
		free(mm);
	}
}

void test_fNORN(){
	uint64_t time; 
	wait(1E9);
	// ------------ notA nor notB = C ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(81920);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+4*(4096+64); // +page size +cache line
		void *out3 = mm+6*(4096+64); // +page size +cache line
		void *out4 = mm+8*(4096+64); // +page size +cache line
		void *out5 = mm+10*(4096+64); // +page size +cache line
		void *out6 = mm+12*(4096+64); // +page size +cache line
		void *out7 = mm+14*(4096+64); // +page size +cache line
		void *out8 = mm+16*(4096+64); // +page size +cache line
		void *out9 = mm+18*(4096+64); // +page size +cache line
		void *out10 = mm+20*(4096+64); // +page size +cache line

		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(in1);
		flush(in2);
		flush(out1);
		flush(out2);
		flush(out3);
		flush(out4);
		flush(out5);
		flush(out6);
		flush(out7);
		flush(out8);
		flush(out9);
		flush(out10);
		
		fence();
		fNORN(in1, in2, out1, out2, out3, out4, out5, out6, out7, out8, out9, out10, out10, out10);
		fence();
		time = probe(out1);	
		printf("fNOrn1 case notA notB: c is %lu\n", time);
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out2);	
		printf("fNOrn2 case notA notB: c is %lu\n", time);
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out3);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out4);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out5);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		time = probe(out6);	
		CU_ASSERT_TRUE(time<THRESHOLD);
		free(mm);
	}
	
	// ------------ A nor notB = notC------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(128000);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+4*(4096+64); // +page size +cache line
		void *out3 = mm+6*(4096+64); // +page size +cache line
		void *out4 = mm+8*(4096+64); // +page size +cache line
		void *out5 = mm+10*(4096+64); // +page size +cache line
		void *out6 = mm+12*(4096+64); // +page size +cache line
		void *out7 = mm+14*(4096+64); // +page size +cache line
		void *out8 = mm+16*(4096+64); // +page size +cache line
		void *out9 = mm+18*(4096+64); // +page size +cache line
		void *out10 = mm+20*(4096+64); // +page size +cache line

		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(in2);
		flush(out1);
		flush(out2);
		flush(out3);
		flush(out4);
		flush(out5);
		flush(out6);
		flush(out7);
		flush(out8);
		flush(out9);
		flush(out10);
		load(in1);
		
		fence();
		fNORN(in1, in2, out1, out2, out3, out4, out5, out6, out7, out8, out9, out10, out10, out10);
		fence();
		time = probe(out1);	
		printf("fNOrn1 case A notB: c is %lu\n", time);
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out3);	
		printf("fNOrn2 case A notB: c is %lu\n", time);
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out2);	
		printf("fNOrn3 case not A: time is %lu\n", time);
        CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out4);	
		printf("fNOrn4 case not A: time is %lu\n", time);
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
		// ------------ notA nor B ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(81920);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+4*(4096+64); // +page size +cache line
		void *out3 = mm+6*(4096+64); // +page size +cache line
		void *out4 = mm+8*(4096+64); // +page size +cache line
		void *out5 = mm+10*(4096+64); // +page size +cache line
		void *out6 = mm+12*(4096+64); // +page size +cache line
		void *out7 = mm+14*(4096+64); // +page size +cache line
		void *out8 = mm+16*(4096+64); // +page size +cache line
		void *out9 = mm+18*(4096+64); // +page size +cache line
		void *out10 = mm+20*(4096+64); // +page size +cache line

		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(in1);
		flush(out1);
		flush(out2);
		flush(out3);
		flush(out4);
		flush(out5);
		flush(out6);
		flush(out7);
		flush(out8);
		flush(out9);
		flush(out10);
		load(in2);
		
		fence();
		fNORN(in1, in2, out1, out2, out3, out4, out5, out6, out7, out8, out9, out10, out10, out10);
		fence();
		time = probe(out1);	
		printf("fNOrn1 case notA B: c is %lu\n", time);
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out3);	
		printf("fNOrn2 case notA B: c is %lu\n", time);
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out4);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		free(mm);
	}
	
		// ------------ A nor B ------------
	for(int i=0;i<CYC;i++){
		void *mm = malloc(81920);
		void *in1=mm;
		void *in2 = mm+4096+64; // +page size +cache line
		void *out1 = mm+2*(4096+64); // +page size +cache line
		void *out2 = mm+4*(4096+64); // +page size +cache line
		void *out3 = mm+6*(4096+64); // +page size +cache line
		void *out4 = mm+8*(4096+64); // +page size +cache line
		void *out5 = mm+10*(4096+64); // +page size +cache line
		void *out6 = mm+12*(4096+64); // +page size +cache line
		void *out7 = mm+14*(4096+64); // +page size +cache line
		void *out8 = mm+16*(4096+64); // +page size +cache line
		void *out9 = mm+18*(4096+64); // +page size +cache line
		void *out10 = mm+20*(4096+64); // +page size +cache line

		*((uint64_t *)in1) =0;
		*((uint64_t *)in2) =0;		
		
		flush(out1);
		flush(out2);
		flush(out3);
		flush(out4);
		flush(out5);
		flush(out6);
		flush(out7);
		flush(out8);
		flush(out9);
		flush(out10);
		load(in1);
		load(in2);
		
		fence();
		fNORN(in1, in2, out1, out2, out3, out4, out5, out6, out7, out8, out9, out10, out10, out10);
		fence();
		time = probe(out1);	
		printf("fNOrn1 case A B: c is %lu\n", time);
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out3);	
		printf("fNOrn2 case A B: c is %lu\n", time);
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out2);	
		CU_ASSERT_TRUE(time>THRESHOLD);
		time = probe(out4);	
		CU_ASSERT_TRUE(time>THRESHOLD);
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


int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Test Suite assignment 3", NULL, NULL);
    int normal = 0;
	
	if(normal){
		CU_add_test(suite, "Test fNOT", test_fNOT);
		CU_add_test(suite, "Test fNOT2", test_fNOT2);
		CU_add_test(suite, "Test fNAND", test_fNAND); 
		CU_add_test(suite, "Test fAND", test_fAND); 
		CU_add_test(suite, "Test fNOR", test_fNOR);
		CU_add_test(suite, "Test fOR", test_fNOR);
		CU_add_test(suite, "Test fXOR", test_fXOR);
	}
	//CU_add_test(suite, "Test fNOTN", test_fNOTN);
    CU_add_test(suite, "Test fNORN", test_fNORN); // TODO

	CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}


