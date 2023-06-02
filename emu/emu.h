#ifndef EMU_H_
#define EMU_H_

#include "../macro.h"

typedef struct Context {
    R_SIZE R[32];
    R_SIZE pc; // instruction but not a pointeur 
    void *code;
    R_SIZE csize;
    char *memory; // virtual memory pointer
} Context ;


typedef struct Itable {
    /* RV32I instruction table  */
    int (*rv32i[10]) (RV32 i, Context *context);
    /* extentions will be here */
} Itable;


int init_context(char *, Context*);
int init_terminal();
int clean_context(Context*);
int exe(Itable *,Context *);
Itable init_itable();
void run(Context *);


#endif
