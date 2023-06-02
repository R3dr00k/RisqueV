#ifndef MEM_H_
#define MEM_H_

#define  MEM_SIZE 0x1000


char *init_vmem(long);

char read_byte(char *, unsigned long);
short read_word(char *, unsigned long);
int read_dword(char *, unsigned long);

void write_byte(char *, unsigned long, char);
void write_word(char *, unsigned long, short);
void write_dword(char *, unsigned long, int);


#endif

