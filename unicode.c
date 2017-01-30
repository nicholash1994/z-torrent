#include "unicode.h"

// returns -1 if unicode string is improperly formatted
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
