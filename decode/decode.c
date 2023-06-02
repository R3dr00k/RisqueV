#include <stdio.h>
#include "decode.h"


char *UNDIFINED = "UNDEF";
char *ARITHM[11] = {"ADD", "SLL" ,"SLT" ,"SLTU", "XOR", "SRL", "OR", "AND","SUB","SRA"};
char *ARITHM_I[9] = {"ADDI", "SLLI","SLTI", "SLTIU", "XORI", "SRLI", "ORI", "ANDI", "SRAI"};
char *LOAD[6] = {"LB", "LH", "LW", "?","LBU", "LHU"};
char *STORE[3] = {"SB", "SH", "SW"};
char *BRANCH[8] = {"BEQ", "BNE", "?", "?", "BLT", "BGE", "BLTU", "BGEU"};
char *OTHER[6] = {"JAL", "JALR", "LUI", "AUIPC", "ECALL", "EBREAK"};

#ifdef RV32M
char *MUL_EXT[8] = {"MUL" ,"MULH", "MULSU", "MULU", "DIV" ,"DIVU", "REM", "REMU"};
#endif

char *ABI_REG[32] = {
   "zero", "ra" ,"sp" , "gp", "tp", "t0" ,
   "t1", "t2","s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9",
  "s10", "s11", "t3", "t4", "t5", "t6"
};


char *arithm_name(int i) {
    int v = funct3(i);
    int f7 = funct7(i);

#ifdef RV32M
    if (f7 == 0x1) {
        return MUL_EXT[v];
    }
#endif

    if (v == 0x0 && f7 == 0x20) {
        /* SUB, 0x20 */
        return ARITHM[8];
    } else if (v == 0x5 && f7 == 0x20) {
        /* SRA, 0x20 */
        return ARITHM[9];
    }

    if ( f7  == 0x0 ) {
        return ARITHM[v];
    } else {
        return UNDIFINED;
    }
}

char *arithm_i_name(int i) {
    int v = funct3(i);
    int imm = BORNED(i_imm(i), 5, 7);

    if (v == 1 && imm != 0x0) {
       return UNDIFINED; 
    } else if (v == 5) {
        if ( imm == 0x20 ) {
            return ARITHM_I[8];
        } else if ( imm != 0 ) {
            return UNDIFINED;
        }
    }
    return ARITHM_I[v];
}

char *load_name(int i) {
    int v = funct3(i);
    if ( v <= 0x5  && v != 0x3) {
        return LOAD[v];
    } else {
        return UNDIFINED;
    }
}

char *store_name(int i) {
    int v = funct3(i);
    if ( v <= 0x2 ) {
        return STORE[v];  
    }

    return UNDIFINED;
}

char *branch_name(int i) {
    int v = funct3(i);
    if (v <= 0x7 && v != 0x2 && v != 0x3) {
        return BRANCH[v];
    }
    return UNDIFINED;
}

void debug_r(int i, char *iname) {
    printf("%s %s, %s, %s\n",iname, ABI_REG[RD(i)] ,ABI_REG[RS1(i)], ABI_REG[RS2(i)]);
}


void debug_i(int i, char *iname, int imm) {
    printf("%s %s, %s, %d\n", iname, ABI_REG[RD(i)], ABI_REG[RS1(i)], imm);
}

void debug_s(int i, char *iname, int imm) {
    printf("%s %s, %s, %d\n", iname, ABI_REG[RS1(i)], ABI_REG[RS2(i)], imm);
}
void debug_u(int i, char *iname, int imm) {
    printf("%s %s, %d\n", iname, ABI_REG[RD(i)], imm);
}
 
/* IMMEDIATE TYPES 32 bit*/
int i_imm(int i) {
    int ret = i >> 20;
    if (i >> 31 ) {
        return SET_BITS(ret, 10, 22);
    } else {
        return ret;
    }
}


int s_imm(int i) {
    int ret = ((i >> 25) << 5) + (( i >> 7) & 31);
    if (i >> 31) {
        return SET_BITS(ret, 10, 22);
    } else {
        return ret;
    }
}

int u_imm(int i) {
    return (i >> 12);
}

int b_imm(int i) {
    int ret = ((i >> 8) & 15) + (((i>>25) & 63) << 4) + (((i >> 7) & 1) << 11);
    if (i >> 31) {
        return SET_BITS(ret, 10, 21) << 1;
    } else {
        return ret << 1;
    }
}

int j_imm(int i) {
    int ret = ((i >> 21) & 1023) + (((i >> 20) & 1) << 10) + (((i >> 12) & 255) << 11) ;
    if (i >> 31) {
        return SET_BITS(ret, 19, 12) << 1;
    } else {
        return ret << 1;
    }
}

/* Generals functions */

void decode_32(int i) {
    int op = OP_32(i);
    int f3 = funct3(i);

    switch ( op ) {
        case 51:
            /* Opcode = 0110011 R-TYPE (arithmetique)*/
            debug_r(i, arithm_name(i));
            break;
        case 0x13:
            /* Opcode = 0010011 I-TYPE (arithmetique immediate)*/
            if (f3 == 0x5 || f3 == 0x1) {
                debug_i(i, arithm_i_name(i), BORNED(i_imm(i), 0, 5));
            } else {
                debug_i(i, arithm_i_name(i), i_imm(i));
            }
            break;
        case 0x3:
            /* Opcode = 0000011 I-TYPE (Load)*/
            if (f3 == 0x0 || f3 == 0x4) {
                debug_i(i, load_name(i), BORNED(i_imm(i), 0, 8));
            } else if (f3 == 0x1 || f3 == 0x5) {
                debug_i(i, load_name(i), BORNED(i_imm(i), 0, 16));
            } else {
                debug_i(i, load_name(i), i_imm(i));
            }
            break;
        case 0x23:
            /* Opcode = 0100011 S-TYPE (Store)*/
            if (f3 == 0x0) {
                debug_s(i, store_name(i), BORNED(s_imm(i), 0, 8));
            } else if (f3 == 0x1) {
                debug_s(i, store_name(i), BORNED(s_imm(i), 0, 16));
            } else {
                debug_s(i, store_name(i), s_imm(i));
            }
            break;
        case 0x63:
            /* Opcode = 1100011 B-TYPE (Branch)*/
            debug_s(i, branch_name(i), b_imm(i));
            break;
        case 0x6F:
            /* Opcode = 1101111 J-TYPE (JAL)*/
            debug_u(i, OTHER[0], j_imm(i));
            break;
        case 0x37:
            /* Opcode = 0110111 U-TYPE (LUI)*/
            debug_u(i, OTHER[2], u_imm(i));
            break;
        case 0x17:
            /* Opcode = 0010111 U-TYPE (AUIPC)*/
            debug_u(i, OTHER[3], u_imm(i));
            break;
        default:
            if (op == 0x67 && f3 == 0x0) {
                /* Opcode = 1100111 I-TYPE (JALR)*/
                debug_i(i, OTHER[1], i_imm(i));
            } else if (op == 0x73 && f3 == 0) {
            /* Opcode = 1110011 I-TYPE 
             * ECALL if funct3 = 0
             * EBREAK else
             * */
                int imm = i_imm(i);
                if (imm == 0) {
                    printf("%s\n", OTHER[4]);
                } else if (imm == 1) {
                    printf("%s\n", OTHER[5]);
                }
            } else {
                printf("UNFOUND OPCODE\n");
            }

    }
    return; 
}

int decode(void *code) {
    char opcode = *(short *)code;

    if ( IS_BITS_NOT_SET(3, opcode) ) {
        fprintf(stderr ,"16 bit is not implemented yet !\n");
        return 2;
    }
    if ( IS_BITS_NOT_SET(28, opcode) ) {
        decode_32(*(RV32 *)code);
        return 4;
    } 

    if ( IS_BIT_SET(32, opcode) ) {
        if ( IS_BIT_SET(64, opcode) ) {
            if ( IS_BITS_NOT_SET(7 , opcode >> 12) )  {
                fprintf(stderr, "%d bit is not implemented yet !\n", 10 + 2 * ((opcode >> 12) & 7));
                return 10 + 2 * ((opcode >> 12) & 7) ;
            } else {
                fprintf(stderr ,"192 bit is not implemented yet !\n");
                return 24;
            }
        } else {
            fprintf(stderr ,"64 bit is not implemented yet !\n");
            return 8;
        }
    } else {
        fprintf(stderr ,"48 bit is not implemented yet !\n");
        return 6;
    }
}


void debug_registers(Context *c) {
    printf("Registers :\n");
    for (int i = 0; i < 32; i ++) {
        printf("%s (x%d) = 0x%x\n", ABI_REG[i], i, c->R[i]);
    }
    printf("\n");
}

void debug_register(Context *c, int i) {
    if ( i >= 0 && i < 32 ) {
        printf("%s (x%d) = 0x%x\n", ABI_REG[i], i, c->R[i]);
    } else {
        printf("Invalid index \n");
    }
}
