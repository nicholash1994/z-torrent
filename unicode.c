#include "unicode.h"
#include <stdint.h>

// returns -1 if unicode string is improperly formatted
int ucharlen(uchar uc) {
	switch (utype(uc)) {
	case UASCII:
		return 1;
		break;
	case UTWO:
		return 2;
		break;
	case UTHREE:
		return 3;
		break;
	case UFOUR:
		return 4;
		break;
	default:
		return -1;
	}
}

int ustrlen(uchar* ustring) {
	int i = 0;
	int j = 0;
	uchar uc;
	while ((uc = ustring[i]) != '\0') {
		if (utype(uc) == -1)
			return -1;
		else if (utype(uc) == UTF)
			return -1;
		else if (utype(uc) == UASCII)
			i+=1;
		else if (utype(uc) == UTWO)
			i+=2;
		else if (utype(uc) == UTHREE)
			i+=3;
		else if (utype(uc) == UFOUR)
			i+=4;
		else
			return -1;
		j++;
	}
	return j;	
}

// returns -1 if not a unicode character
int utype(uchar uc) {
	if ((uc&128) == 0)
		return UASCII;
	else if ((uc&192) == 128)
		return UTF;
	else if ((uc&224) == 192)
		return UTWO;
	else if ((uc&240) == 224)
		return UTHREE;
	else if ((uc&248) == 240)
		return UFOUR;
	else {
		fprintf(stderr, "Error: unicode string formatted incorrectly!\n");
		return -1;
	}
}

void print_uchar(uchar* uc) {
	printf("U+%04X\n", get_uval(uc));
}

unsigned int get_uval(uchar* uc) {
	unsigned int uval;

	uval = 0;
	
	uchar bytes[4];
	if (utype(uc[0]) == UASCII)
		uval = uc[0];
	else if (utype(uc[0]) == UTWO) {
		printf("DEBUG: two-byte character\n");
		uval += (uc[1] & 63);
		uval += ((unsigned int)(uc[0] & 31)) << 6;
	}
	else if (utype(uc[0]) == UTHREE) {
		printf("DEBUG: three-byte character\n");
		uval += uc[2]&63;
		uval += uc[1]&63 << 6;
		uval += uc[0]&15 << 10;
	}
	else if (utype(uc[0]) == UFOUR) {
		printf("DEBUG: four-byte character\n");
		uval += (uint32_t)uc[3]&63;
		uval += ((uint32_t)uc[2]&63) << 6;
		uval += ((uint32_t)uc[1]&63) << 12;
		uval += ((uint32_t)uc[0]&7) << 15;
	}
	else {
		fprintf(stderr, "Error: UTF-8 character improperly formatted!\n");
		return -1;
	}
	
	return uval;
}

// just a wrapper for strcpy
uchar* ustrcpy(uchar* dest, const uchar* src) {
	return strcpy(dest, src);
}

// returns null if there was an error
uchar* ustrncpy(uchar* dest, const uchar* src, size_t num) {
	// character index
	int i, k;
	// byte index
	int j = 0;

	if (dest == NULL)
		return NULL;
	
	for (i = 0; i < num; i++)
		if (utype(src[j]) == UASCII)
			// j++ might have to be put on the next line
			dest[j++] = src[j];
		else if (utype(src[j]) == UTWO) 
			for (k = 0; i < 2; k++)
				dest[j++] = src[j];
		else if (utype(src[j]) == UTHREE)
			for (k = 0; k < 3; k++)
				dest[j++] = src[j];
		else if (utype(src[j]) == UFOUR)
			for (k = 0; k < 4; k++)
				dest[j++] = src[j];
		else
			return NULL;

	return dest;
}
