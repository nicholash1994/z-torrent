#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "unicode.h"

void scan_unicode_string();

struct torrent {

};

union bval {
	uchar* val;
	struct bdict* dict;
};

struct bdict {
	uchar* key;
	union bval val;
	struct bdict* next;
};


struct bdict read_torrent_file(const char* filename);

int main() {
	FILE* torrent_file;
	uchar uc;
	int i;
	unsigned int n;

	torrent_file = fopen("./ubuntu.torrent", "rb");
	printf("File opened successfully!\n");


	fclose(torrent_file);

	return 0;
} 

void scan_unicode_string() {
	uchar unicode_string[80];
	uchar uc;
	int i;

	// Prompt user for unicode string
	printf("Enter unicode string: ");
	while ((uc = getchar()) != '\n' && i < 80)
		unicode_string[i++] = uc;
	unicode_string[i] = '\0';
	
	printf("Byte length of unicode string: %lu\n", strlen(unicode_string));	
	printf("Character length of unicode string: %u\n", ustrlen(unicode_string));
}

/*  
struct bdict read_torrent_file(const char* filename) {
	FILE* torrent;
	uchar uc;
	uchar key_name[30];
	int done_reading_file;

	torrent = fopen(filename, "rb");
	if (torrent = NULL) {
		fprintf(stderr, "Error: torrent file couldn't be read!\n");
		exit(-1);
	}

	done_reading_file = 0;	
	while (!done_reading_file) {
		read_key(torrent, key_name);
		strncmp(key_name, "announce", 30);
	}
}
*/

void read_key(FILE* file, uchar* dest) {
	int i;
	uchar uc;
	
	i = 0;
	while ((uc=fgetc(file)) != ':' && i < 30)
		dest[i++] = uc;
	dest[i] = '\0';
}

