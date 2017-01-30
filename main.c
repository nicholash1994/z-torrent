#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "unicode.h"

int main() {
	uchar* unicode_string;
	uchar uc;
	int i = 0;

	unicode_string = (uchar*)malloc(80*sizeof(uchar));
	while ((uc = getchar()) != '\n' && i < 80)
		unicode_string[i++] = uc;
	unicode_string[i] = '\0';
	
	printf("Byte length of unicode string: %d\n", strlen(unicode_string));	
	printf("Character length of unicode string: %d\n", ustrlen(unicode_string));

	return 0;
} 
