#include "des.h"
#include "des_top.h"

void initDES(unsigned int *K, unsigned int *Keys) {
  unsigned int KP[2] = {0,0};
  unsigned int CD[34];

  zeroOut(CD);
  permuteKP(K, KP, CD);
  setupBlocks(CD);
  formKeys(CD, Keys);
  return;
}

void DES(unsigned char *input, unsigned int *Keys, unsigned int *IIP, unsigned int decrypt) {
  unsigned int M[2];
  unsigned int IP[34];

  *(IIP) = 0;
  *(IIP+1) = 0;
  zeroOut(IP);
  setupM(input, M);
  initialPermute(M, IP);
  swap(IP, Keys, decrypt);
  finalPermute(&IP[16], IIP);
  return;
}

void finalPermute(unsigned int *IP, unsigned int *Inv_IP) {
  int i, j;
  char bit;
  for(i=0;i<64;i++) {
    j = Inverse_IP_shift[i];
    if(j <= 32) {
      bit = (*(IP+17) >> (32-j)) & 0x1;
    }
    else {
      bit = (*(IP) >> (32-(j-32))) & 0x1;
    }

    if(i < 32) {
      *(Inv_IP) |= bit << (31-i);
    }
    else {
      *(Inv_IP+1) |= bit << (31-(i-32));
    }
  }
  return;
}

unsigned int f(unsigned int Right, unsigned int *K) {
  unsigned int i, j = 0;
  unsigned int s = 0;
  unsigned int e[2] = {0, 0};

  /* E */
  for(i=0;i<48;i++) {
    if(i < 24) {
      *(e) |= (((Right >> (32-E_shifts[i])) & 0x1) << (23-i));
    }
    else {
      *(e+1) |= (((Right >> (32-E_shifts[i])) & 0x1) << (23-(i+8)));
    }
  }

  /* XOR */
  *(e) = *(e) ^ *(K);
  *(e+1) = *(e+1) ^ *(K+17);

  /* SBoxes */
  s |= S1[(((((*(e) >> 23) & 0x1) << 1) | ((*(e) >> 18) & 0x1)) * 16) + ((*(e) >> 19) & 0xf)] << 28;
  s |= S2[(((((*(e) >> 17) & 0x1) << 1) | ((*(e) >> 12) & 0x1)) * 16) + ((*(e) >> 13) & 0xf)] << 24;
  s |= S3[(((((*(e) >> 11) & 0x1) << 1) | ((*(e) >>  6) & 0x1)) * 16) + ((*(e) >>  7) & 0xf)] << 20;
  s |= S4[(((((*(e) >>  5) & 0x1) << 1) | ((*(e))       & 0x1)) * 16) + ((*(e) >>  1) & 0xf)] << 16;

  s |= S5[(((((*(e+1) >> 23) & 0x1) << 1) | ((*(e+1) >> 18) & 0x1)) * 16) + ((*(e+1) >> 19) & 0xf)] << 12;
  s |= S6[(((((*(e+1) >> 17) & 0x1) << 1) | ((*(e+1) >> 12) & 0x1)) * 16) + ((*(e+1) >> 13) & 0xf)] <<  8;
  s |= S7[(((((*(e+1) >> 11) & 0x1) << 1) | ((*(e+1) >>  6) & 0x1)) * 16) + ((*(e+1) >>  7) & 0xf)] <<  4;
  s |= S8[(((((*(e+1) >>  5) & 0x1) << 1) | ((*(e+1))       & 0x1)) * 16) + ((*(e+1) >>  1) & 0xf)];

  /* Permute */
  for(i=0;i<32;i++) {
    j |= ((s >> (32-(P_shift[i]-32))) & 0x1) << (31 - i);
  }
  return j;
}

void swap(unsigned int *IP, unsigned int *K, unsigned int decrypt) {
  int i;
  for(i=1;i<17;i++) {
    *(IP+i) = (*((IP+17)+(i-1)));
    if(!decrypt) {
      *((IP+17)+i) = (*(IP+(i-1))) ^ f((*((IP+17)+(i-1))), (K+i));
    }
    else {
      *((IP+17)+i) = (*(IP+(i-1))) ^ f((*((IP+17)+(i-1))), (K+(17-i)));
    }
  }
}

void initialPermute(unsigned int *M, unsigned int *IP) {
  int i, j;
  char bit;
  for(i=0;i<64;i++) {
    j = IP_shift[i];
    if(j <= 32) {
      bit = ((*(M) >> (32-j)) & 0x1);
    }
    else {
      bit = ((*(M+1) >> (32-(j-32))) & 0x1);
    }
    if(i < 32) {
      *(IP) |= (bit << (31-i));
    }
    else {
      *(IP+17) |= (bit << (31-(i-32)));
    }
  }
  return;
}

void setupM(unsigned char *input, unsigned int *M) {
  int i = 0;
  unsigned char this;
  *M = 0;
  *(M+1) = 0;
  do {
    this = *input++;
    if(i < 4) {
      *M |= (this & 0xff) << (24 - (8*i));
    }
    else {
      *(M+1) |= (this & 0xff) << (24 - (8*(i-4)));
    }
  } while(++i < 8);
  return;
}

void formKeys(unsigned int *CD, unsigned int *K) {
  int i, j, l;
  char bit;
  for(i=1;i<17;i++) {
    for(j=0;j<48;j++) {
      l = PC_2[j];
      if(l <= 28) {
	bit = ((*(CD+i)) >> (28-l) & 0x1);
      }
      else {
	bit = (((*((CD+17)+i))) >> (28-(l-28))) & 0x1;
      }
      if(j < 24) {
	*(K+i) |= (bit << (23-j));
      }
      else {
	*((K+17)+i) |= (bit << (23-(j+8)));
      }
    }
  }
  return;
}

void zeroOut(unsigned int *P) {
  int i;
  for(i=0;i<34;i++) {
    *(P+i) = 0x0;
  }
  return;
}

void setupBlocks(unsigned int *CD) {
  int i;
  for(i=1;i<17;i++) {
    *(CD+i) = ((*(CD+(i-1))) << shifts[i-1]) | ((*(CD+(i-1))) >> (28 - shifts[i-1]));
    *((CD+17)+i) = ((*((CD+17)+(i-1))) << shifts[i-1]) | ((*((CD+17)+(i-1))) >> (28 - shifts[i-1]));
  }
}

void permuteKP(unsigned int *K, unsigned int *KP, unsigned int *CD) {
  int i, j;
  char bit;
  for(i=0;i<56;i++) {
    j = PC_1[i];
    if(j <= 32) {
      bit = ((*(K) >> (32-j)) & 0x1);
    }
    else {
      bit = ((*(K+1) >> (32-(j-32))) & 0x1);
    }

    if(i < 28) {
      *(KP) |= (bit << (31-i));
      *CD |= (bit << (27-i));
    }
    else {
      *(KP + 1) |= (bit << (31-(i+4)));
      *(CD + 17) |= (bit << (27-(i+4)));
    }
  }
  return;
}
