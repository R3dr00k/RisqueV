#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include "../../macro.h"
#include "../../emu/emu.h"
#include "../../decode/decode.h"

#define RV32I 1

int load(RV32, Context *);
int arithm_i(RV32, Context *);
int store(RV32, Context *);
int arithm(RV32, Context *);
int jal(RV32, Context *);
int jalr(RV32, Context *);
int branch(RV32, Context *);
int sys(RV32, Context *);
int lui(RV32, Context *);
int auipc(RV32, Context *);

#endif
