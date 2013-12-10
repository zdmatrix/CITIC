#include "stdafx.h"
#include <stdio.h>
#include <malloc.h>
#include <fcntl.h>
char sbox1[]=
  {14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7,
    0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8,
    4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0,
   15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13
  };
char sbox2[]=
  {15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10,
    3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5,
    0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15,
   13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9
  };
char sbox3[]=
  {10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8,
   13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1,
   13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7,
    1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12
  };
char sbox4[]=
  { 7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15,
   13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9,
   10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4,
    3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14
  };
char sbox5[]=
  { 2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9,
   14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6,
    4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14,
   11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3
  };
char sbox6[]=
  {12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11,
   10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8,
    9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6,
    4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13
  };
char sbox7[]=
  { 4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1,
   13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6,
    1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2,
    6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12
  };
char sbox8[]=
  {13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7,
    1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2,
    7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8,
    2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11
  };
char *sbox[]={sbox1,sbox2,sbox3,sbox4,sbox5,sbox6,sbox7,sbox8};
char table1[]= /* for initial permutation */
    {58,50,42,34,26,18,10, 2,60,52,44,36,28,20,12, 4,
     62,54,46,38,30,22,14, 6,64,56,48,40,32,24,16, 8,
     57,49,41,33,25,17, 9, 1,59,51,43,35,27,19,11, 3,
     61,53,45,37,29,21,13, 5,63,55,47,39,31,23,15, 7
    };
char table2[]= /* for permutation choice 1 */
    {57,49,41,33,25,17, 9,    1,58,50,42,34,26,18,
     10, 2,59,51,43,35,27,   19,11, 3,60,52,44,36,
     63,55,47,39,31,23,15,    7,62,54,46,38,30,22,
     14, 6,61,53,45,37,29,   21,13, 5,28,20,12, 4
    };
char table3[]= /* for expansion function */
    {32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9,10,11,
     12,13,12,13,14,15,16,17,16,17,18,19,20,21,20,21,
     22,23,24,25,24,25,26,27,28,29,28,29,30,31,32, 1
    };
char table4[]= /* for permutation choice 2 */
    {14,17,11,24, 1, 5, 3,28,15, 6,21,10,23,19,12, 4,
     26, 8,16, 7,27,20,13, 2,41,52,31,37,47,55,30,40,
     51,45,33,48,44,49,39,56,34,53,46,42,50,36,29,32
    };
char table5[]= /* for inverse of initial permutation */
    {40, 8,48,16,56,24,64,32,39, 7,47,15,55,23,63,31,
     38, 6,46,14,54,22,62,30,37, 5,45,13,53,21,61,29,
     36, 4,44,12,52,20,60,28,35, 3,43,11,51,19,59,27,
     34, 2,42,10,50,18,58,26,33, 1,41, 9,49,17,57,25
    };
char table6[]= /* for permutation function P */
    {16, 7,20,21,29,12,28,17, 1,15,23,26, 5,18,31,10,
      2, 8,24,14,32,27, 3, 9,19,13,30, 6,22,11, 4,25
    };
char table7[]= /* for selection function */
    { 1, 6, 2,1, 7,12, 8,5,13,18,14,9,19,24,20,13,
     25,30,26,17,31,36,32,21,37,42,38,25,43,48,44,29
    };
char table8[]= /* for shift left 1 */
    { 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,
     18,19,20,21,22,23,24,25,26,27,28, 1,30,31,32,33,
     34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,
     50,51,52,53,54,55,56,29
    };
char table9[]= /* for shift left 2 */
    { 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,
     19,20,21,22,23,24,25,26,27,28, 1, 2,31,32,33,34,
     35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,
     51,52,53,54,55,56,29,30
    };
char table10[]= /* for shift right 1 */
    {28, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
     16,17,18,19,20,21,22,23,24,25,26,27,56,29,30,31,
     32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
     48,49,50,51,52,53,54,55,
    };
char table11[]= /* for shift right 2 */
    {27,28, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
     15,16,17,18,19,20,21,22,23,24,25,26,55,56,29,30,
     31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,
     47,48,49,50,51,52,53,54
    };
char *table[]=
    {table1,table2,table3,table4,table5,table6,
     table7,table8,table9,table10,table11
    };
/* content is table index,total count */
unsigned char initial_permutation[]={0,64};
unsigned char permutation_c1[]={1,56};
unsigned char expansion_function[]={2,48};
unsigned char permutation_c2[]={3,48};
unsigned char inverse_ip[]={4,64};
unsigned char permutation_fp[]={5,32};
unsigned char shift_left1[]={7,56};
unsigned char shift_left2[]={8,56};
unsigned char shift_right1[]={9,56};
unsigned char shift_right2[]={10,56};
unsigned char x64[8],i64[8],k64[8],k56[7],k48[6],l32[4],r48[6],r32[4];
void transdata(unsigned char *input,unsigned char *output,unsigned char *function);
void fnct(unsigned char *io,unsigned char *input);
void select(unsigned char *input,unsigned char *output);

void transdata(unsigned char *input,unsigned char *output,unsigned char *function)
  {register int i;
   unsigned char out[8];
   for (i=0;i<function[1]>>3;i++)
     out[i]=0;               /* initial value with 0 */
   for (i=0;i<function[1];i++)
     out[i>>3] |= ((input[table[function[0]][i]-1 >> 3]
                     & (0x80>>(table[function[0]][i]-1 & 0x7)))
                     << (table[function[0]][i]-1 & 0x7)) >> (i & 0x7);
   for (i=0;i<function[1]>>3;i++)
     output[i]=out[i];          /* initial value with 0 */
   return;
  }
void select(unsigned char *input,unsigned char *output)
  {int idx,i,j,k,cnt;
   for (i=0;i<4;i++)
     output[i]=0;               /* initial value with 0 */
   cnt=0;
   for (i=0;i<8;i++)
     {
      for (j=5,k=0,idx=0;j>=0;j--)
        {idx+=((input[table7[cnt]-1+k>>3] & (0x80>>(table7[cnt]-1+k & 0x7)))
                ? 1 <<j : 0);
         if (j>3)
            cnt++;
         else
           k++;
        }
      cnt++;
      for (j=0;j<4;j++)
        output[table7[cnt]+j-1 >> 3] |= ((sbox[i][idx] & (0x08 >> j)) << 4+j)
                                        >> (table7[cnt]+j-1 & 0x7);
      cnt++;
     }
     return ;
  }
void fnct(unsigned char *io,unsigned char *input)/* io->r32 input->k56 */
  {unsigned char k48[6],r48[6],s32[4];
   register int i;
   transdata(input,k48,permutation_c2); /* 28+28 to 48 */
   transdata(io,r48,expansion_function); /* 32 to 48 */
   for (i=0;i<6;i++)    /* expn xor key */
     r48[i] ^=k48[i];
   select(r48,s32);
   transdata(s32,io,permutation_fp);
   return ;
  }

void encrypt(unsigned char *input,unsigned char *key,unsigned char *output)
  {unsigned char i64[8],k56[7],t32[4];
   transdata(input,i64,initial_permutation);
   transdata(key,k56,permutation_c1);
/*****shift left 1 ****/
   transdata(k56,k56,shift_left1);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 1 ****/
   transdata(k56,k56,shift_left1);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 2 ****/
   transdata(k56,k56,shift_left2);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 2 ****/
   transdata(k56,k56,shift_left2);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 2 ****/
   transdata(k56,k56,shift_left2);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 2 ****/
   transdata(k56,k56,shift_left2);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 2 ****/
   transdata(k56,k56,shift_left2);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 2 ****/
   transdata(k56,k56,shift_left2);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 1 ****/
   transdata(k56,k56,shift_left1);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 2 ****/
   transdata(k56,k56,shift_left2);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 2 ****/
   transdata(k56,k56,shift_left2);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 2 ****/
   transdata(k56,k56,shift_left2);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 2 ****/
   transdata(k56,k56,shift_left2);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 2 ****/
   transdata(k56,k56,shift_left2);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 2 ****/
   transdata(k56,k56,shift_left2);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/*****shift left 1 ****/
   transdata(k56,k56,shift_left1);
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   fnct(i64+4,k56);
   i64[4] ^= i64[0]; i64[5] ^= i64[1]; i64[6] ^= i64[2]; i64[7] ^= i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
/* substitute */
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   i64[4]=i64[0]; i64[5]=i64[1]; i64[6]=i64[2]; i64[7]=i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
   transdata(i64,output,inverse_ip);
   return;
  }
void decrypt(unsigned char *input,unsigned char *key,unsigned char *output)
  {unsigned char i64[8],k56[7],t32[4];
   transdata(input,i64,initial_permutation);
/* substitute */
   t32[0]=i64[4]; t32[1]=i64[5]; t32[2]=i64[6]; t32[3]=i64[7];
   i64[4]=i64[0]; i64[5]=i64[1]; i64[6]=i64[2]; i64[7]=i64[3];
   i64[0]=t32[0]; i64[1]=t32[1]; i64[2]=t32[2]; i64[3]=t32[3];
   transdata(key,k56,permutation_c1);
/*****shift right 1 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right1);
/*****shift right 2 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right2);
/*****shift right 2 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right2);
/*****shift right 2 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right2);
/*****shift right 2 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right2);
/*****shift right 2 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right2);
/*****shift right 2 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right2);
/*****shift right 1 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right1);
/*****shift right 2 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right2);
/*****shift right 2 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right2);
/*****shift right 2 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right2);
/*****shift right 2 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right2);
/*****shift right 2 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right2);
/*****shift right 2 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right2);
/*****shift right 1 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right1);
/*****shift right 1 ****/
   t32[0]=i64[0]; t32[1]=i64[1]; t32[2]=i64[2]; t32[3]=i64[3];
   fnct(i64,k56);
   i64[0] ^= i64[4]; i64[1] ^= i64[5]; i64[2] ^= i64[6]; i64[3] ^= i64[7];
   i64[4]=t32[0]; i64[5]=t32[1]; i64[6]=t32[2]; i64[7]=t32[3];
   transdata(k56,k56,shift_right1);
/* substitute */
   transdata(i64,output,inverse_ip);
   return;
  }


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/////////////////////////////3DES//////////////////////////////////////////
void TripleDES(unsigned char *input, unsigned char *key, unsigned char *output)
{
    unsigned char tmpbuf1[16];
    unsigned char tmpbuf2[16];

    encrypt(input,&key[16],tmpbuf1);
    decrypt(tmpbuf1,&key[8],tmpbuf2);
    //encrypt(tmpbuf2,key,output);   
	encrypt(tmpbuf2,key,output);   

    return;
}
void TripleDES_1(unsigned char *input, unsigned char *key, unsigned char *output)
{
    unsigned char tmpbuf1[16];
    unsigned char tmpbuf2[16];

    decrypt(input,&key[16],tmpbuf1);
    encrypt(tmpbuf1,&key[8],tmpbuf2);
    //decrypt(tmpbuf2,key,output);   
	decrypt(tmpbuf2,key,output);   
    return;
}

void cal_plain_key(unsigned char *indata,unsigned char *key,unsigned char *outdata)
{
   int i,j;

   j=strlen((char *)indata);
   for(i=0;i<(j/8);i++) TripleDES_1(indata+i*8,key,outdata+i*8);
}

void Disperse(unsigned char *cardnumber,unsigned char *key,unsigned char *OutPut)
{
   int i;    

   TripleDES(cardnumber,key,OutPut);
   for(i=0; i<16; i++)
     cardnumber[i] = ~cardnumber[i];

   TripleDES(cardnumber,key,&OutPut[8]);
   return;
}         

void cal_SMdata(unsigned char *indata,int len,unsigned char *key,unsigned char *SMdata)
{
  int i,newlen;
  unsigned char inbuf[256];
 
  memset(inbuf,0,sizeof(inbuf));
  inbuf[0]=len;
  memcpy(inbuf+1,indata,len);
  len++;  
  inbuf[len] = 0x80;
  newlen = len + 8 - len%8;

  for(i=0;i<newlen/8;i++)  TripleDES(inbuf+i*8,key,SMdata+i*8);
  return;
}

void calMAC_3DES(unsigned char *input, int len, unsigned char *key, unsigned char *output)
{
  int i,j,newlen;
  unsigned char inbuf[128];
  unsigned char tmpbuf[128];
  unsigned char tmpbuf1[128];
 
  memset(inbuf,0,sizeof(inbuf));
  memcpy(inbuf,input,len);
  
  inbuf[len] = 0x80;
  newlen = len + 8 - len%8;
     
  memset(tmpbuf,0,128);
  memcpy(tmpbuf,inbuf,8);
  for(i=1; i<(newlen/8)-1; i++)
  {
     for(j=0; j<8; j++)
        tmpbuf[j] = tmpbuf[j] ^ inbuf[i*8 +j];

     encrypt(tmpbuf,key,tmpbuf1);
     memcpy(tmpbuf,tmpbuf1,8);
  }
  for(j=0; j<8; j++)
        tmpbuf[j] = tmpbuf[j] ^ inbuf[((newlen/8)-1)*8 +j];
  TripleDES(tmpbuf,key,tmpbuf1);
  memcpy(output,tmpbuf1,8);
  return;
}