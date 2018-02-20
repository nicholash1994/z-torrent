#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "unicode.h"

enum val_type {USTRING, BDICT, BINT};

struct parser_state {
	char dts[100];
	int dts_i;
};

union bval {
	char* val;
	struct bdict* dict;
	int32_t b_int;
};

// can represent either a list (if the key pointer is NULL)
// or a dictionary (if it's not)
struct bdict {
	char* key;
	union bval val;
	// indicates whether bval describes a utf-8 string or another bdict
	int vtype;
	struct bdict* next;
	struct bdict* parent;
};

void read_dict(struct bdict* dict, FILE* file, int* r_depth);
void read_list(struct bdict* dict, FILE* file, int* r_depth);
void read_int(struct bdict* dict, FILE* file, int* r_depth);
char* read_elem(FILE* file);
struct bdict* read_torrent_file(const char* filename);
void parser_ctrl(struct bdict* curr, FILE* torrent, int* r_depth);

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

/*
struct bdict* read_torrent_file(const char* filename) {
	FILE* torrent;
	uchar uc;
	uchar key_name[30];
	struct bdict* root_dict;
	struct bdict* curr;
	int r_depth;

	torrent = fopen(filename, "rb");
	if (torrent == NULL) {
		fprintf(stderr, "Error: torrent file couldn't be read!\n");
		exit(-1);
	}
	
	r_depth = 0;
	curr = root_dict;
	
	while (1)
		switch (uc=fgetc(torrent)) {
		case 'd':
			r_depth++;
			read_dict(curr, torrent, &r_depth);
			break;
		case 'l':
			r_depth++;
			read_list(curr, torrent, &r_depth);
			break;
		case 'i':
			r_depth++;
			read_int(curr, torrent, &r_depth);
			break;
		case 'e':
			r_depth--;
		default:
			return root_dict;
		}
	}


}
*/



// returns NULL if the torrent file
// is improperly formatted
struct bdict* read_torrent_file(const char* filename) {
	FILE* torrent;
	uchar uc;
	uchar key_name[30];
	int r_depth;
	struct bdict root_dict;
	struct bdict* curr;

	torrent = fopen(filename, "rb");
	if (torrent == NULL) {
		fprintf(stderr, "Error: torrent file couldn't be read!\n");
		exit(-1);
	}

	// this parses the dictionary
	parser_ctrl(&root_dict, torrent, &r_depth);

}

// may change the name of this function
void parser_ctrl(struct bdict* curr, FILE* torrent, int* r_depth) {
	uchar uc;

	switch (uc=fgetc(torrent)) {
		case 'd':
			*r_depth++;
			read_dict(curr, torrent, r_depth);
			break;
		case 'l':
			*r_depth++;
			read_list(curr, torrent, r_depth);
			break;
		case 'i':
			*r_depth++;
			read_int(curr, torrent, r_depth);
			break;
		case 'e':
			*r_depth--;
		default:
			return;
	}
}




void read_dict(struct bdict* dict, FILE* file, int* r_depth) {
	uchar uc;
	int depth; // the recursion depth of the dictionary
			   // that this function is parsing
	
	// storing the recursion depth of the dictionary
	// that's being read
	depth = *r_depth;
	
	// creating a dictionary as the value of the current dictionary
	// and then navigating to it
	dict->val.dict = (struct bdict*)malloc(sizeof(struct bdict));
	dict->val.dict->parent = dict;
	dict = dict->val.dict;
	dict->vtype = BDICT;

	// setting the key
	dict->key = read_elem(file);

	while (1) {
		// reading the next character
		uc=fgetc(file);
		fseek(file, -1, SEEK_CUR);
		
		// checking to see whether the value is a UTF-8 string
		// or another datatype which is to be read by the parser
		if (uc >= '0' && uc <= '9') {
			dict->val.val = read_elem(file);
		}
		else {
			parser_ctrl(dict, file, r_depth);
		}

		// if the last character which was read was 'e', then
		// *r_depth will be less than depth when parser_ctrl
		// returns, which allows us to exit the loop (and the function)
		if (depth > *r_depth) {
			dict->next = NULL;
			return;
		}
		
		// if there are more elements in the dictionary that
		// need to be read, the following code just creates the
		// next node in the dictionary, and navigates to it
		dict->next = (struct bdict*)malloc(sizeof(struct bdict));
		dict = dict->next;
		dict->key = read_elem(file);
		dict->vtype = BDICT;
	}
}

void read_list(struct bdict* dict, FILE* file, int* r_depth) {
	uchar uc;
	int depth; // the recursion depth of the list
			   // that this function is parsing
	
	// storing the recursion depth of the list
	// that's being read
	depth = *r_depth;
	
	// creating a dictionary as the value of the current dictionary
	// and then navigating to it
	dict->val.dict = (struct bdict*)malloc(sizeof(struct bdict));
	dict->val.dict->parent = dict;
	dict = dict->val.dict;
	dict->vtype = BDICT;

	// the only difference between a list and a dictionary
	// (in terms of how they're implemented in this program)
	// is that a list is simply a dictionary with they keys
	// set to NULL.
	dict->key = NULL;

	while (1) {
		// reading the next character
		uc=fgetc(file);
		fseek(file, -1, SEEK_CUR);

		// checking to see whether the value is a UTF-8 string
		// or another datatype which is to be read by the parser
		if (uc >= '0' && uc <= '9') {
			(dict->val).val = read_elem(file);
		}
		else {
			parser_ctrl(dict, file, r_depth);
		}

		// if the last character which was read was 'e', then
		// *r_depth will be less than depth when parser_ctrl
		// returns, which allows us to exit the loop (and the function)
		if (depth > *r_depth)
			return;

		// if there are more elements in the dictionary that
		// need to be read, the following code just creates the
		// next node in the dictionary, and navigates to it
		dict->next = (struct bdict*)malloc(sizeof(struct bdict));
		dict = dict->next;
		dict->key = NULL;
		dict->vtype = BDICT;
	}
}


// strings returned by this function are
// dynamically allocated and must be manually
// freed
//
// TODO: the "pieces" dictionary entry has a value which
// is not a regular UTF-8 string. This function needs to 
// be modified to parse it correctly
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

	size = strtoul(size_string, NULL, 10);
	elem = (char*)malloc((1+size)*sizeof(char));

	for (i = 0; i < size; i++)
		elem[i] = fgetc(file);
	elem[size] = '\0';

	return elem;
}

void read_int(struct bdict* dict, FILE* file, int* r_depth) {
	
}
