#include "command.h"
#include <stdlib.h>
#include <string.h>
#include "../decode/decode.h"

void help() {
    printf("help : get some help\n");
    printf("debug <register index> : print register value, if there is no register specified all the registers are debugged.\n");
    printf("next : execute a single instruction \n");
    printf("run : execute all instruction until the end\n");
    printf("quit : exit the emulator. \n");
    printf("code : print one line of assembly. ");
}

void debug(Context *c, char *arg) {
    for ( int i = 0; i < strlen(arg); i ++ ) {
        if ( arg[i] != *" " ) {
            debug_register(c, atoi(arg + i));
            return;
        }

    }
    debug_registers(c);
}

int next(Context *c, Itable *it) {
    int ret = exe(it, c);
    if (ret < 0) {
        // WE may want to change the force exit ?
        clean_context(c);
        exit(-1);
    } else {
        c->pc += ret; 
        if (c->pc >= c->csize) {
            // end
            return 1;
        }

    }
    return 0;
}

void code(Context *c, char *arg) {
    // change HERE
    for ( int i = 0; i < strlen(arg); i ++ ) {
        if ( arg[i] != *" " ) {
            debug_register(c, atoi(arg + i));
            return;
        }

    }
    debug_registers(c);
}
