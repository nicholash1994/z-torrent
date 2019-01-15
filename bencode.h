/*
	I used an association list to implement the bencoded dictionary data
	structure because most bencoded dictionaries that are used by torrent programs 
        are small and have less
	than 20 records in total. Not only that, but bencoded dictionaries often 
	have dictionaries as entries, i.e. subdictionaries. So if I used a hash
	table, for example, I would have to create multiple hash tables whenever
	opening a file, and whenever receiving a message from a tracker. Even if
	using a hash table would make the program faster, it would probably be 
	only be a small margin. However, I might be wrong, and I may consider
	switching to a hash table in the future.
*/ 

#ifndef _BENCODE_H
#define _BENCODE_H

#include <stdio.h>
#include <sys/types.h>

#define PEEK_AHEAD(uc, file) {uc=fgetc(file); fseek(file, -1, SEEK_CUR);}

#define INTCEIL(n, d) ((n%d==0 ? n/d : n/d + 1))

enum val_type {USTRING, BDICT, BINT};

/* 
 * This struct is needed because strings in 
 * bencoded dictionaries aren't always NULL-terminated.
 * For example, the pieces string in pretty much every
 * torrent metafile is not NULL-terminated (and can't be,
 * since it's in binary, not plaintext). Thus, we need to
 * store the length of the string as well
 */
struct b_string {
		char* val;
		u_int64_t len;
};

union bval {
	struct b_string b_string;
	struct bdict* dict;
	int64_t b_int;
};

/* can represent either a list (if the key pointer is NULL)
 or a dictionary (if it's not) */
struct bdict {
	struct b_string key;
	union bval val;
	/* indicates whether bval describes a utf-8 string or another bdict */
	int vtype;
	struct bdict* next;
	struct bdict* parent;
};

void read_dict(struct bdict* dict, FILE* file);
void read_list(struct bdict* dict, FILE* file);
void read_int(struct bdict* dict, FILE* file);
struct b_string read_elem(FILE* file);
struct bdict* read_torrent_file(const char* filename);
void parser_ctrl(struct bdict* curr, FILE* torrent);
void print_bdict(struct bdict* dict);
void print_bdict_h(struct bdict* dict, int depth);
void print_record(struct bdict* dict, int depth);
void encode_bdict(struct bdict* dict, FILE* output);
void encode_bdict_h(struct bdict* dict, FILE* output);
int destroy_bdict(struct bdict* dict);
struct bdict* find_bdict(struct bdict* root, const char* key_path);
struct bdict* get_bdict(struct bdict* root, char** key_path);


#endif
