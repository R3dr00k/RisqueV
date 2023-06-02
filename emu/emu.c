#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "command.h"
#include "emu.h"
#include "mem.h"
#include "../instructions/RV32I/instructions.h"

#define CODE_SIZE_LIMIT 0x1000

int init_code(char *name, void **ptr, R_SIZE *size) {
    // allocate space to put code inside of file "name"
    // we may
    //  need to add a code pointeur inside of the context.
    struct stat st;
    FILE *code = fopen(name, "r");

    if (fstat(fileno(code), &st) != 0) {
        return -1;
    }
    
    *size = st.st_size;
    if (*size > CODE_SIZE_LIMIT) {
        *size = CODE_SIZE_LIMIT;
    }

    *ptr = malloc(*size);

    int r = fread(*ptr, 1, *size, code);


    if ( r != *size ) {
        fprintf(stderr, "got different size ! size : %d , fread : %d\n", *size, r);
        fclose(code);
        free(*ptr);
        return -1;
    }

    fclose(code);
    return 0;
}

int clean_code(void *code, R_SIZE size) {
    memset(code, 0, size);
    free(code);
    return 0;
}

int init_context(char *instructions, Context *context) {
    /* put 0 in registers */
    memset(context->R, 0, 32 * sizeof(R_SIZE));


    if (init_code(instructions, &context->code, &context->csize) != 0) {
        fprintf(stderr, "failed to initialize code\n");
        return -1;
    } 

    if (context->csize == -1) {
        return -1;
    } 

    context->pc = 0;

    context->memory = init_vmem(MEM_SIZE);
    if (context->memory == NULL) {
        fprintf(stderr, "failed to initialize vmem\n");
        return -1;
    }
    return 0;
}

int clean_context(Context *context) {
    int code = 0;
    clean_code(context->code, context->csize);

    if (munmap(context->memory, MEM_SIZE) != 0) {
        fprintf(stderr, "Failed to munmap sorry ;).");
        code = -1; 
    }
    return code;
}


int exe(Itable *itable, Context *c) {
    /* call function for 32 bit instruction */
    /* itable = {LOAD, ARITHM_I, STORE, ARITHM, 
     * JAL, JALR, BRANCH, SYS, LUI, AUIPC}*/
    int ret = 0;
    short opcode = *(short *)(c->code + c->pc);

    if ((opcode & 0x3 ) != 0x3) {
        // 16 bit
        fprintf(stderr, "16 bit instruction arent implemented yet.\n");
        return 2;

    } else if ((opcode & 28) != 28) {
        // 32 bit
        RV32 i = *(RV32 *)(c->code + c->pc);
        int op = OP_32(i);
        if ((op & 0xF) == 0x3) {
            // not all the values (4 & 5 missing)
            if ((op >> 4) != 4 || (op >> 4) != 5) {
                ret = itable->rv32i[op >> 4](i, c);
                return 4;
            } 
        } 
        switch (op) { 
            case 0x6F:
                /* JAL */
                ret = itable->rv32i[4] (i, c);
                break;
            case 0x67:
                /* JALR */
                ret = itable->rv32i[5] (i, c);
                break;
            case 0x37:
                /* LUI */
                ret = itable->rv32i[8] (i, c);
                break;
            case 0x17:
                /* AUIPC */
                ret = itable->rv32i[9] (i, c);
                break;
            default:
                return -1;
        }
        if (ret == -1) {
            fprintf(stderr, "Error while executing 32bit instruction\n");
            return -1;
        }
        return 4;
    }
    
    return 0;
}

Itable init_itable() {
    Itable ret;

    ret.rv32i[0] = load;
    ret.rv32i[1] = arithm_i;
    ret.rv32i[2] = store;
    ret.rv32i[3] = arithm;
    ret.rv32i[4] = jal;
    ret.rv32i[5] = jalr;
    ret.rv32i[6] = branch;
    ret.rv32i[7] = sys;
    ret.rv32i[8] = lui;
    ret.rv32i[9] = auipc;

    return ret;
}

int init_terminal() {
    return setvbuf(stdout, NULL, _IONBF, 0);
}

void run(Context *c) {
    char input_buffer[32];
    char *arg = NULL;
    char cont = 1;
    Itable it = init_itable();

    while ( cont ) {
        printf("> ") ;
        if (fgets(input_buffer, 32, stdin) == NULL) {
            fprintf(stderr, "Failed to read command!");
        } else {
            // parse here
            for (int i = 0; i < 32; i++) {
                if (input_buffer[i] == ' ' || input_buffer[i] == '\n' ) {
                    input_buffer[i] = 0;
                    arg = input_buffer + i + 1;
                    break;

                }
            }

            if ( strcmp(input_buffer, "help") == 0) {
                help();
            } else if ( strcmp(input_buffer, "debug") == 0) {
                debug(c, arg);
            } else if ( strcmp(input_buffer, "next") == 0) {
                if (next(c, &it) == 1)  {
                    //end
                    printf("END OF CODE\n");
                    debug_registers(c);
                    cont = 0;
                }                
            } else if ( strcmp(input_buffer, "quit") == 0 ) {
                cont = 0;
                printf("Goodbye\n");
            } else if (strcmp(input_buffer, "code") == 0) {
                decode(c->code + c->pc);
            }
        }
        memset(input_buffer, 0, 32);
    }
}




