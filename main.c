#include <stdio.h>
#include "des_top.h"
#include "input.h"
#include <arpa/inet.h>
#include <string.h>

int main(int argv, char *argc[]) {
  unsigned int IIP[2];
  int i;
  FILE *fp;
  char *filename = NULL;
  int in[2];
  unsigned int K[2] = {0x13345779, 0x9bbcdff1}; /* default key value */
  unsigned int Keys[34];
  unsigned int decrypt = 0;
  unsigned int hexout = 1;

  for(i=1;i<argv;i++) {
    if(!strcmp(argc[i], "key")) {
      /* get the next two values */
      /* TODO */
      sscanf(argc[++i], "%u", &K[0]);
      sscanf(argc[++i], "%u", &K[1]);
    }
    else if(!strcmp(argc[i], "decrypt")) {
      decrypt = 1;
    }
    else if(!strcmp(argc[i], "dec")) {
      hexout = 0;
    }
    else {
      /* file name */
      filename = argc[i];
    }
  }

  zeroOut(Keys);
  initDES(K, Keys);

  /*  DES(input, Keys, IIP);
  printf("0x%08x", IIP[0]);
  printf("%08x\n", IIP[1]);*/

  /*DES(input2, Keys, IIP);
  printf("0x%08x", *(IIP));
  printf("%08x\n", *(IIP+1));*/

  if(filename == NULL) {
    for(i=0;i<16;i++) {
      DES(&input[i][0], Keys, IIP, decrypt);
      printf("0x%08x%08x\n", IIP[0], IIP[1]);
    }
  }
  else {
    if( (fp = fopen(filename, "r")) == NULL ) {
      printf("Could not open input file (%s)\n", filename);
      return -1;
    }

    while(!feof(fp)) {
      if(fscanf(fp, "%d", &in[0]) < 0) {
	break;
      }
      if(fscanf(fp, "%d", &in[1]) < 0) {
	break;
      }
      in[0] = ntohl(in[0]); /* flip endian */
      in[1] = ntohl(in[1]);
      DES((unsigned char *)&in[0], Keys, IIP, decrypt);
      if(hexout) {
	printf("0x%08x%08x\n", IIP[0], IIP[1]);
      }
      else {
	printf("%u\n%u\n", IIP[0], IIP[1]);
      }
    }
  }

  return 0;
}

