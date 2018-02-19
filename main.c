#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "unicode.h"

int main() {
	uchar* unicode_string;
	uchar uc;
	int i = 0;

	unicode_string = (uchar*)malloc(80*sizeof(uchar));

	// Prompt user for unicode string
	printf("Enter unicode string: ");
	while ((uc = getchar()) != '\n' && i < 80)
		unicode_string[i++] = uc;
	unicode_string[i] = '\0';
	
	printf("Byte length of unicode string: %lu\n", strlen(unicode_string));	
	printf("Character length of unicode string: %u\n", ustrlen(unicode_string));

	return 0;
} 
