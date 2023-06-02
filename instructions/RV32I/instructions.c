#include <stdio.h>
#include "instructions.h"
#include "../../emu/mem.h"
#include "../../macro.h"

#define A c->R[RD(i)]
#define B c->R[RS1(i)]
#define C c->R[RS2(i)]

#define INV_OP -1
#define INV_IMM -2
#define U unsigned int

int load(RV32 i, Context *c) {
    switch (funct3(i)) {
        case 0:
            /* LB */
            A = read_byte(c->memory, B + i_imm(i));
            break;
        case 1:
            /* LH */
            A = read_word(c->memory, B + i_imm(i));
            break;
        case 2:
            /* LW */
            A = read_dword(c->memory, B + i_imm(i));
            break;
        case 4:
            /* LBU */
            // unset last bit
            A = (read_byte(c->memory, B+i_imm(i)) << 1) >> 1;
            break;
        case 5:
            /* LHU */
            // unset last bit
            A = (read_word(c->memory, B+i_imm(i)) << 1) >> 1;
        default:
            return INV_OP;
    }
    return 0;
}

int arithm_i(RV32 i, Context *c) {
    switch (funct3(i)) {
        case 0:
            //ADDI
            printf("i : %d\n, a : %d\nb : %d\nimm: %d\n",i, RD(i), RS1(i), i_imm(i));
            A = B + i_imm(i);
            break;
        case 1:
            if ( BORNED(i_imm(i), 5, 11) == 0x0) {
                A = B << BORNED(i_imm(i), 0, 4);
            } else { return INV_OP; }
            break;
        case 2:
            A = ((int)B < i_imm(i)) ? 1 : 0;  
            break;
        case 3:
            A = (B < (U)i_imm(i)) ? 1 : 0;  
            break; 
        case 4:
            A = B ^ i_imm(i);
            break;
        case 5:
            if ( BORNED(i_imm(i), 5, 11) == 0x0) {
                /* SRLI */
                A = B >> BORNED(i_imm(i), 0, 4);
            } else if ( BORNED(i_imm(i), 5, 11) == 0x20 ) { 
                /* SRAI */
                A = B >> BORNED(i_imm(i), 0, 4);
            } else {return INV_OP;}
            break;
        case 6:
            A = B | i_imm(i);
            break;
        case 7:
            A = B & i_imm(i);
            break;
        default:
            return INV_OP;

    }
    return 0;
}

int store(RV32 i, Context *c) {
    switch (funct3(i)) {
        case 0x0:
            break;
            write_byte(c->memory, B + s_imm(i), (char)C);
        case 0x1:
            write_word(c->memory, B + s_imm(i), (short)C);
            break;
        case 0x2:
            write_dword(c->memory, B + s_imm(i), C);
            break;
        default:
            return INV_OP;
    }
    return 0;
}

/* All arithmetics functions R2R */
int arithm(RV32 i, Context *c) {
    if (funct7(i) == 0x0) {
        switch (funct3(i)) {
            case 0:
                // ADD
                A = B + C;
                break;
            case 0x1:
                // SLL
                A = B << C;
                break;
            case 0x2:
                // SLT 
                A = ((int)B < (int)C) ? 1 : 0;
                break;
            case 0x3:
                // SLTU 
                A = (B < C) ? 1 : 0;
                break;
            case 0x4:
                // XOR
                A = B ^ C;
                break;
            case 0x5:
                // SRL
                A = B >> C;
                break;
            case 0x6:
                // OR
                A = B | C;
                break;
            case 0x7:
                // AND
                A = B & C;
                break;
        }
    } else if (funct7(i) == 0x20) {
        if ( funct3(i) == 0x0 ) {
            // SUB
            A = B - C;
        }  else if (funct3(i) == 0x5) {
            //SRA
            A = B >> C;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
} 

int jal(RV32 i, Context *c) {
    if (c->pc + j_imm(i) <= c->csize - 4) {
        A = c->pc + 4; 
        c->pc += j_imm(i);
    } else {
        return -1;
    }
    return 0;
}

int jalr(RV32 i, Context *c) {
    if (!funct3(i)) {
        if (B + i_imm(i) <= c->csize - 4) {
            A = c->pc + 4;
            c->pc = B + i_imm(i);
        }
    } 
    return INV_OP;
}

int branch(RV32 i, Context *c) {
    if (c->pc + b_imm(i) <= c->csize - 4) {
        switch (funct3(i)) {
            case 0x0:
                if ( B == C ) c->pc += b_imm(i);
                break;
            case 0x1:
                if ( B != C ) c->pc += b_imm(i);
                break;
            case 0x4:
                if ( (int)B < (int)C ) c->pc += b_imm(i);
                break;
            case 0x5:
                if ( (int)B >= (int)C ) c->pc += b_imm(i);
                break;
            case 0x6:
                if ( B > C ) c->pc += b_imm(i);
                break;
            case 0x7:
                if ( B <= C ) c->pc += b_imm(i);
                break;
            default:
                return INV_OP;
        }
        return 0;
    }
    return INV_IMM;
}

int sys(RV32 i, Context *c) {
    if (i_imm(i) == 0x0 || i_imm(i) == 0x1) {
        printf("Not impl yet\n");
        return 0;
    }
    return INV_OP;
}

int lui(RV32 i, Context *c) {
    A = u_imm(i); 
    return 0;
}

int auipc(RV32 i, Context *c) {
    // may overflow ?
    if (c->pc + u_imm(i) <= c->csize - 4) {
        A = c->pc + u_imm(i); 
        return 0;
    }
    return INV_IMM;
}







