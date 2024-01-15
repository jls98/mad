#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "aes.h"


#define AESSIZE 16
#define PLAINTEXT_WIDTH 32
#define AMOUNT_PLAINTEXTS 10000

typedef uint8_t aes_t[AESSIZE];


// TODO fix clobbers
static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    asm volatile ("rdtscp" : "=A" (x) : : "edx", "ecx");
    return x;
}

void tobinary(char *data, aes_t aes) {
  assert(strlen(data)==AESSIZE*2);
  unsigned int x;
  for (int i = 0; i < AESSIZE; i++) {
    sscanf(data+i*2, "%2x", &x);
    aes[i] = x;
  }
}

char *toString(aes_t aes) {
  char buf[AESSIZE * 2 + 1];
  for (int i = 0; i < AESSIZE; i++)
    sprintf(buf + i*2, "%02x", aes[i]);
  return strdup(buf);
}

void randaes(aes_t aes) {
  for (int i = 0; i < AESSIZE; i++)
    aes[i] = rand() & 0xff;
}

int aes_encrypt_call_with_time_measurement(aes_t input, aes_t output, AES_KEY key)
{
	uint64_t start = rdtsc();
	int a=0;
	while(rdtsc()-start < 1000000000){a++;}
	start = rdtsc();
	AES_encrypt(input, output, &key);
	return rdtsc() - start;
}


int call_aes(char *key_input, char *plaintext_input) {
  aes_t key, input, output;
  tobinary(key_input, key);
  AES_KEY aeskey;
  private_AES_set_encrypt_key(key, 128, &aeskey);
  
  /*printf("aes key is \n");
  for (size_t i=0; i<aeskey.rounds;i++)
  {
	  printf("rd key %i is %i\n", i, aeskey.rd_key[i]);
  }*/

  tobinary(plaintext_input, input);
  return aes_encrypt_call_with_time_measurement(input, output, aeskey);
  /*char *x = toString(output);
  printf("%s\n", x);
  free(x);*/
  //return 0;
}

// Function to generate a random hexadecimal string of a given length
char *generateRandomHexadecimal(int length) {
    const char hexChars[] = "0123456789abcdef";
    char* hexString = (char*)malloc((length + 1) * sizeof(char)); // +1 for the null terminator
    if (hexString == NULL) {
        perror("Memory allocation error");
        exit(1);
    }

    //srand((unsigned int)time(NULL));

    for (int i = 0; i < length; i++) {
        int randomIndex = rand() % 16;
        hexString[i] = hexChars[randomIndex];
    }

    hexString[length] = '\0';
    return hexString;
}



int analyzer()
{
	char *generated_string;
	char static_key[]= "2b7e151628aed2a6abf7158809cf4f3c";
	uint32_t measurements[AMOUNT_PLAINTEXTS] = {0};
	
	for (size_t i=0; i<AMOUNT_PLAINTEXTS; i++){
		generated_string = generateRandomHexadecimal(PLAINTEXT_WIDTH);
		measurements[i] = call_aes(static_key, generated_string);
	}
	
	// detect outliers
	
	double mean =0.0f; // mean
	for (size_t i=0; i<AMOUNT_PLAINTEXTS; i++) mean+=measurements[i];
	mean/=AMOUNT_PLAINTEXTS;
	printf("the mean is %f\n", mean);
	
	double deviation =0.0f; // standard deviation
	for (size_t i=0; i<AMOUNT_PLAINTEXTS; i++) deviation+=sqrt(measurements[i]-mean);
	deviation/=AMOUNT_PLAINTEXTS;
	printf("the standard deviation is %f\n", deviation);
	
	
	
	
	return 0;
}

int main(int ac, char **av)
{
	/*task 1*/
	//call_aes("2b7e151628aed2a6abf7158809cf4f3c", "6bc1bee22e409f96e93d7e117393172a"); // test vector
		
	/*task 2*/
	analyzer();
	
	return 0;
}
