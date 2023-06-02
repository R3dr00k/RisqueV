#ifndef DECODE_H_
#define DECODE_H_

#include "../macro.h"
#include "../emu/emu.h"

int i_imm(int);
int s_imm(int);
int b_imm(int);
int u_imm(int);
int j_imm(int);

int decode(void *);
void debug_registers(Context*);
void debug_register(Context*, int i);


#endif
