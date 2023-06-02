#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "../macro.h"
#include "emu.h"

void help();
void debug(Context *, char *);
int next(Context *c, Itable *);

#endif 
