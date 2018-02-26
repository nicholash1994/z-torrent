#ifndef _BENCODE_H
#define _BENCODE_H

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

typedef struct bdict_stack {
	struct bdict** stack;
	int size, block_size;
} bdict_stack_t;

void read_dict(struct bdict* dict, FILE* file, int* r_depth);
void read_list(struct bdict* dict, FILE* file, int* r_depth);
void read_int(struct bdict* dict, FILE* file, int* r_depth);
char* read_elem(FILE* file);
struct bdict* read_torrent_file(const char* filename);
void parser_ctrl(struct bdict* curr, FILE* torrent, int* r_depth);
void print_bdict(struct bdict* dict);
void print_bdict_h(struct bdict* dict, int depth);
void print_record(struct bdict* dict);
char* get_announce_url(struct bdict* dict);
void encode_bdict(struct bdict* dict, FILE* output);
void init_bdict_stack(bdict_stack_t* stack, int block_size);
void destroy_bdict_stack(bdict_stack_t*);
struct bdict* pop_bdict(bdict_stack_t*);
void push_bdict(bdict_stack_t*, struct bdict*);



#endif
