#include "mem.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>


char * init_vmem(long size) {
    char * mem_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,0,0 ); 
    if ( mem_ptr == MAP_FAILED ) {
        return NULL;
    } 
    return mem_ptr;
}


char read_byte(char *mem,unsigned long offset) {
    /* offset must be in [0, size - 1]*/ 
    if (offset > MEM_SIZE - 1) {
        // segfault        
        fprintf(stderr, "Reading out of bound");
        exit(0);
    }
    return mem[offset];
}

short read_word(char *mem,unsigned long offset) {
    /* offset must be in [0, size - 2]*/ 
    if (offset > MEM_SIZE - 2) {
        // segfault        
        fprintf(stderr, "Reading out of bound");
        exit(0);
    }
    return *(short *)(mem + offset);
}

int read_dword(char *mem,unsigned long offset) {
    /* offset must be in [0, size - 2]*/ 
    if (offset > MEM_SIZE - 4) {
        // segfault        
        fprintf(stderr, "Reading out of bound");
        exit(0);
    }
    return *(int *)(mem + offset);
}

void write_byte(char *mem,unsigned long offset, char v) {
    /* offset must be in [0, size - 2]*/ 
    if (offset > MEM_SIZE - 1) {
        // segfault        
        fprintf(stderr, "Writing out of bound");
        exit(0);
    }
    mem[offset] = v;
}
void write_word(char *mem,unsigned long offset, short v) {
    /* offset must be in [0, size - 2]*/ 
    if (offset > MEM_SIZE - 2) {
        // segfault        
        fprintf(stderr, "Writing out of bound");
        exit(0);
    }
    *(short *)(mem + offset) = v;
}

void write_dword(char *mem,unsigned long offset, int v) {
    /* offset must be in [0, size - 2]*/ 
    if (offset > MEM_SIZE - 4) {
        // segfault        
        fprintf(stderr, "Writing out of bound");
        exit(0);
    }
    *(int *)(mem + offset) = v;
}
