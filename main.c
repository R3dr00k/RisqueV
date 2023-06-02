#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "decode/decode.h"
#include "emu/emu.h"
#include "emu/mem.h"


int main(int argc, char **argv) {
    Context c;
    
    if (argc != 2) {
        printf("%s <file>\n", argv[0]);
        return -1;
    }

    if (init_terminal() != 0) {
        fprintf(stderr, "Failed to initialize terminal !\n");
        return -1;
    }

    if (init_context(argv[1], &c) != 0) {
        fprintf(stderr, "failed to initialize context !\n");
        return -1;
    }
    
    /* Run the emulator */
    run(&c);
    
    /* clean context structure */
    clean_context(&c);

    return 0;
}
