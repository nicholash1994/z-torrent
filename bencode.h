#ifndef __BENCODE_H
#define __BENCODE_H

#define PEEK_AHEAD(uc, file) {uc=fgetc(file); fseek(file, -1, SEEK_CUR);}

enum val_type {USTRING, BDICT, BINT};

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
void print_bdict(struct bdict* dict);
void print_bdict_h(struct bdict* dict, int depth);
void print_record(struct bdict* dict);


#endif
