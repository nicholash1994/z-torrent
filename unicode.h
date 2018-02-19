#include <stdio.h>
#include <string.h>

enum utype { UASCII, UTF, UTWO, UTHREE, UFOUR };

typedef unsigned char uchar;

// returns -1 if unicode string is improperly formatted
int ustrlen(uchar* ustring);

// returns -1 if not a unicode character
int utype(uchar uc);

// just a wrapper for strcpy
uchar* ustrcpy(uchar* dst, const uchar* src);

// returns null if there was an error
uchar* ustrncpy(uchar* dest, const uchar* src, size_t num);

void print_uchar(uchar* uc);

unsigned int get_uval(uchar* uc);
