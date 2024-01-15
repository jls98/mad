#include <stdio.h>


// t3
int getRandomNumber() {
  int result;
  asm volatile ("mov %0, 4": "=r" (result));
  return result;
}

// t4
int add(int a, int b) {
  int result;
  asm volatile ("add %1, %2": "=r" (result): "r" (a), "r" (b));
  return result;
}

// t5
int sum(int *a, int n) {
  int result;
  asm volatile (
		  "mov %0 [%1+4*%2]		\n"
		  "dec %2				\n"
		"loop: 					\n"
		  "mov %%rdx [%1+4*%2]	\n"
		  "add %0 %%rdx			\n"
		  "dec %2				\n"
		  "jns loop"
		: "=r" (result)
		: "r" (a), "r" (n)
		: "rdx");
  return result;
}


// t6
void func() {
    int c  = 0;

    for (int i = 0; i < 10; i++) {
      c++;
      asm volatile("");
    }
}

int main() {
	int rng = getRandomNumber();
	printf("RNG is %i\n", rng);
	
	int summand1 = 1, summand2 = 2;
	
	int sums = add(summand1, summand2);
	printf("sum of %i and %i is %i\n", summand1, summand2, sums);
	
	
	int n = 3;
	int summands[3] = {1, 2, 3};
	sums = sum(summands, n);
	printf("sum of %i, %i and %i is %i\n", summands[0], summands[1], summands[2], sums);
	
	

	
}
