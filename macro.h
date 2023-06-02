#ifndef MACRO_H_
#define MACRO_H_

#include <stdio.h>
#include <limits.h>

/* TYPES ALIASES */
#define RV32 unsigned int  // instruction
#define R_SIZE unsigned int // registers 
#define R_SIZE_MAX UINT_MAX  // registers 

/* Macros to get field of 32 bit instruction */
#define OP_32(I) I & 127
#define funct3(I) ((I >> 12 ) & 7)
#define funct7(I) ((I >> 25) & 127)
#define RD(I) (I >> 7) & 31
#define RS1(I) (I >> 15) & 31
#define RS2(I) (I >> 20) & 31

/*If all of the bits of BITS ar */
#define IS_BITS_NOT_SET(BITS, N)  (N & BITS)^BITS 

/*If the bit is not set return 0 */
#define IS_BIT_SET(BIT, N) N & BIT

/* UNSET or SET bits start at Offset, N time to the left*/
#define UNSET_BITS(V, O, N) (~((0xFFFFFFFF >> (32-N)) << O)) & V
#define SET_BITS(V, O, N) ~(~((0xFFFFFFFF >> (32-N)) << O) & (~V))

#define BORNED(V, O, N) (((0xFFFFFFFF >> (32-N))) & (V >> O))

#endif
