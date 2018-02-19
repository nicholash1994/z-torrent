#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "unicode.h"

enum val_type {USTRING, BDICT, BINT};

struct parser_state {
	char dts[100];
	int dts_i;
}

union bval {
	uchar* val;
	struct bdict* dict;
	int32_t b_int;
};

// can represent either a list (if the key pointer is NULL)
// or a dictionary (if it's not)
struct bdict {
	uchar* key;
	union bval val;
	// indicates whether bval describes a utf-8 string or another bdict
	int vtype;
	struct bdict* next;
	struct bdict* parent;
};


struct bdict read_torrent_file(const char* filename);

int main() {
	FILE* torrent_file;
	uchar utf8char[4];

	torrent_file = fopen("./ubuntu.torrent", "rb");
	if (torrent_file == NULL) {
		fprintf(stderr, "Error! Torrent file couldn't be opened!\n");
		return -1;
	}
		

	fclose(torrent_file);

	return 0;
} 


// returns NULL if the torrent file
// is improperly formatted
struct bdict* read_torrent_file(const char* filename) {
	FILE* torrent;
	uchar uc;
	uchar key_name[30];
	struct parser_state ps;
	struct bdict* root_dict;
	struct bdict* curr;
	int dict_depth; // dictionary depth
				    // so if we're just reading a dictionary,
					// the dictionary depth will be 1
					// but if we're reading a dictionary inside
				    // a dictionary, for example, then the dictionary depth
				    // would be 2

	torrent = fopen(filename, "rb");
	if (torrent == NULL) {
		fprintf(stderr, "Error: torrent file couldn't be read!\n");
		exit(-1);
	}
	
	ps.dts_i = 0;	
	if ((uc=fgetc(torrent)) != 'd')
		return NULL:
	else {
		ps.dts[ps.dts_i++] = 'd';

	root_dict = (struct bdict*)malloc(sizeof(struct bdict));
	curr = root_dict;

	while () {
		switch (uc=fgetc(torrent)) {
		case 'd':
			ps.dts[ps.dts_i++] = uc;
			read_dict(curr, &ps);
			break;
		case 'l':
			ps.dts[ps.dts_i++] = uc;
			read_list(curr, &ps);
			break;
		case 'i':
			ps.dts[ps.dts_i++] = uc;
			read_int(curr, &ps);
			break;
		case 'e':
			ps.dts_i--;
		default:
			return NULL;
		}
	}
}

void parser_ctrl(FILE* torrent, struct parser_state* ps) {
	uchar uc;

	switch (uc=fgetc(torrent)) {
		case 'd':
			ps->dts[ps->dts_i++] = uc;
			read_dict(curr, torrent, &ps);
			break;
		case 'l':
			ps->dts[ps->dts_i++] = uc;
			read_list(curr, &ps);
			break;
		case 'i':
			ps->dts[ps->dts_i++] = uc;
			read_int(curr, &ps);
			break;
		case 'e':
			ps->dts_i--;
		default:
			return;
	}
}

void read_dict(struct bdict* dict, FILE* file, struct parser_state* ps) {
	uchar uc;
	int r_depth;
	
	r_depth = ps->dts_i;
	
	dict->val.dict = (struct bdict*)malloc(sizeof(struct bdict));
	dict->val.dict->parent = dict;
	dict = dict->val.dict;

	dict->key = read_elem(file);

	while (1) {
		if (uc >= '0' && uc <= '9') {
			fseek(file, -1, SEEK_CUR);
			dict->val.val = read_elem(file);
		}
		else
			parser_ctrl(file, ps);


		if (r_depth > ps->dts_i)
			return;
		dict->next = (struct bdict*)malloc(struct bdict);
		dict = dict->next;
		dict->key = read_elem(file);
	}
}

// strings returned by this function are
// dynamically allocated and must be manually
// freed
char* read_elem(FILE* file) {
	int i;
	uchar uc;
	char size_string[30];
	char* elem;
	unsigned long size;
	
	i = 0;
	while ((uc=fgetc(file)) != ':' && i < 30)
		size_string[i++] = uc;
	size_string[i] = '\0';

	size = strtoul(size_string);
	elem = (char*)malloc((1+size)*sizeof(char));

	for (i = 0; i < size; i++)
		elem[i] = fgetc(file);
	elem[size] = '\0';

	return elem;
}

