#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t public[] = "Public message";
uint8_t secret[1024] = "Secret";
uint8_t space1[512]={1};
int public_len = sizeof(public);
uint8_t space2[512]={1};

int victim(uint64_t index) {
  if (index >= public_len)
    return -1;
  return public[index];
}