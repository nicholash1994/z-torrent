#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "bencode.h"
#include "error.h"
#include "macros.h"

extern int errno;

/*
 * returns NULL if the torrent file
 * is improperly formatted
 */
struct bdict* read_torrent_file(const char* filename) {
	FILE* torrent;
	char uc; char key_name[30];
	struct bdict* root_dict;

	torrent = fopen(filename, "rb");
	if (torrent == NULL)
		err(NULL, "Error: couldn't open torrent file!\n");
	
	MALLOC(root_dict, struct bdict, 1);
	CLEAR(root_dict, struct bdict, 1);

	/* this parses the dictionary */
	parser_ctrl(root_dict, torrent);

	fclose(torrent);

	return root_dict;

}

/*
 * when a special character is encountered, (e.g. 'd', 'e', 'l')
 * this function decides what to do next
 */
void parser_ctrl(struct bdict* curr, FILE* torrent) {
	char uc;

	switch (uc=fgetc(torrent)) {
		case 'd':
			curr->vtype = BDICT;
			read_dict(curr, torrent);
			break;
		case 'l':
			curr->vtype = BDICT;
			read_list(curr, torrent);
			break;
		case 'i':
			curr->vtype = BINT;
			read_int(curr, torrent);
			break;
		default:
			return;
	}
}

/*
 * This function reads a bencoded dictionary. Since entries in bencoded
 * dictionaries can also have dictionaries as their value, this function
 * plays a critical role in parsing torrent files.
 */
void read_dict(struct bdict* dict, FILE* file) {
	char uc;

	/*
	 * creating a dictionary as the value of the current dictionary
	 * and then navigating to it
	 */
	MALLOC(dict->val.dict, struct bdict, 1);
	CLEAR(dict->val.dict, struct bdict, 1);
	/*
	 * setting the previous dictionary (the one passed as
	 * an argument to this function) as the parent of the newly
	 * created dictionary
	 */
	dict->val.dict->parent = dict;
	dict = dict->val.dict;

	/* setting the key */
	dict->key = read_elem(file);

	while (1) {
		/* reading the next character */
		PEEK_AHEAD(uc, file);
		
		/*
		 * checking to see whether the value is a UTF-8 string
		 * or another datatype which is to be read by the parser
		 */
		if (uc >= '0' && uc <= '9') {
			dict->vtype = USTRING;
			dict->val.val = read_elem(file);
		}
		else {
			parser_ctrl(dict, file);
		}
		
		/*
		 * if there are more elements in the dictionary that
		 * need to be read, the following code just creates the
		 * next node in the dictionary, and navigates to it
		 */
		PEEK_AHEAD(uc, file);
		if (uc != 'e') {
			MALLOC(dict->next, struct bdict, 1);
			dict->next->parent = dict->parent;
			dict = dict->next;
			dict->key = read_elem(file);
		}
		else {
			fseek(file, 1, SEEK_CUR);
			dict->next = NULL;
			break;
		}
	}
}

void read_list(struct bdict* dict, FILE* file) {
	char uc;
	
	/*
	 * creating a dictionary as the value of the current dictionary
	 * and then navigating to it
	 */
	MALLOC(dict->val.dict, struct bdict, 1);
	CLEAR(dict->val.dict, struct bdict, 1);

	dict->val.dict->parent = dict;
	dict = dict->val.dict;

	/*
	 * the only difference between a list and a dictionary
	 * (in terms of how they're implemented in this program)
	 * is that a list is simply a dictionary with it's entries' keys
	 * set to NULL.
	 */
	dict->key = NULL;

	while (1) {
		/* reading the next character */
		PEEK_AHEAD(uc, file);

		/*
		 * checking to see whether the value is a UTF-8 string
		 * or another datatype which is to be read by the parser
		 */
		if (uc >= '0' && uc <= '9') {
			dict->vtype = USTRING;
			(dict->val).val = read_elem(file);
		}
		else {
			/*
			 * parser control will also determine the type of 
			 * record dict is
			 */
			parser_ctrl(dict, file);
		}

		/*
		 * if there are more elements in the dictionary that
		 * need to be read, the following code just creates the
		 * next node in the dictionary, and navigates to it
		 */
		PEEK_AHEAD(uc, file);
		if (uc != 'e') {
			/*
			 * creating the next record in the dictionary
			 * and navigating to it
			 */
			MALLOC(dict->next, struct bdict, 1);
			CLEAR(dict->next, struct bdict, 1);
			dict->next->parent = dict->parent;
			dict = dict->next;
			dict->key = NULL;
		}
		else {
			/* setting the next pointer to NULL and exiting the loop */
			fseek(file, 1, SEEK_CUR);
			dict->next = NULL;
			break;
		}
	}
}


/*
 * strings returned by this function are
 * dynamically allocated and must be manually
 * freed
 */
char* read_elem(FILE* file) {
	int i;
	char uc;
	char size_string[30];
	char* elem;
	unsigned long size;
	
	/*
	 * this section of code simply copies the length of the element
	 * into a buffer
	 */
	i = 0;
	while ((uc=fgetc(file)) != ':' && i < 30)
		size_string[i++] = uc;
	size_string[i] = '\0';

	/*
	 * this just converts the size_string to an actual integer
	 * and then allocates a string to store the actual element
	 */
	size = strtoul(size_string, NULL, 10);
	elem = (char*)malloc((1+size)*sizeof(char));

	for (i = 0; i < size; i++)
		elem[i] = fgetc(file);
	elem[size] = '\0';

	return elem;
}

/*
 * BEP 3 says that integers in bencoded dictionaries
 * can be arbitrarily large. However, this program can 
 * only process integers that can fit into a long value.
 * If a bencoded dictionary contains an integer that can't
 * fit into a long value, the program will print an error message 
 * and exit. In the future, I may add support for arbitrarily 
 * large numbers in order to make z-torrent fully BEP 3 
 * compliant.
 */
void read_int(struct bdict* dict, FILE* file) {
	char c;
	int i;
	/* According to BEP 3, all integers in bencoded
	 * dictionaries are in decimal and the largest integer
	 * that can be put in a long variable is
	 * 4,294,967,295. Thus, 10 characters are needed for the 
	 * number itself, an additional character for a sign character,
	 * and one final character for the NULL byte
	 */
	char buffer[12];

	i = 0;
	while ((c=fgetc(file)) != 'e' && i < 11)
		buffer[i++] = c;
	buffer[i] = '\0';
	
	dict->vtype = BINT;
	dict->val.b_int = strtol(buffer, NULL, 10);
	if (errno == ERANGE) {
		fprintf(stderr, "Error: integer in bencoded dictionary too large!\n");
		exit(-ERANGE);
	}
}

void print_bdict(struct bdict* dict) {
	print_bdict_h(dict->val.dict, 0);
}

void print_bdict_h(struct bdict* dict, int depth) {

	print_record(dict, depth);
	if (dict->vtype == BDICT && dict->val.dict != NULL)
		print_bdict_h(dict->val.dict, depth+1);
	if (dict->next != NULL)
		print_bdict_h(dict->next, depth);

	return;
}
	
void print_record(struct bdict* dict, int depth) {
	int i;

	/* Spaces for indentation */
	for (i = 0; i < 2*depth; i++) fputc(' ', stdout);

	if (dict->key == NULL)
		printf("Key: NULL\n");
	else
		printf("Key: %s\n", dict->key);

	/* Spaces for indentation */
	for (i = 0; i < 2*depth; i++) fputc(' ', stdout);

	switch (dict->vtype) {
	case BINT:
		printf("Value: %d\n\n", dict->val.b_int);
		break;
	case BDICT:
		printf("Value: dictionary/list\n\n");
		break;
	case USTRING:
		printf("Value: %s\n\n", dict->val.val);
		break;
	}
}

void encode_bdict(struct bdict* dict, FILE* output) {
	encode_bdict_h(dict->val.dict, output);
}

void encode_bdict_h(struct bdict* dict, FILE* output) {
	long len;
	long piece_len;
	struct bdict* tmp;
	
	/*
	 * This if-else statement just checks to see if we're
	 * iterating over a list or a dictionary, and prints
	 * and prints "d" if it's a dictionary and "l" if it's
	 * a regular list
	 */
	if (dict->key != NULL) {
		fprintf(output, "d");
	}
	else {
		fprintf(output, "l");
	}

	/* Iterating through each element in the dictionary */
	while (dict != NULL) {
	
		/* Printing key if it exists */
		if (dict->key != NULL)
			fprintf(output, "%d:%s", strlen(dict->key), dict->key);
	
		/* Iterating through the dictionary (or list) */
		switch(dict->vtype) {
		case BDICT:
			/* 
			 * If a nested dictionary or list is found, 
			 * make a recursive call to encode_bdict_h
			 */
			encode_bdict_h(dict->val.dict, output);
			break;
		case BINT:
			fprintf(output, "i%lde", dict->val.b_int);
			break;
		case USTRING:
			/* 
			 * Pieces string requires special handling because it's 
			 * not NULL-terminated 
			 */
			if (dict->key != NULL && strcmp(dict->key, "pieces") == 0) {
				/* Getting the length of the file */
				if ((tmp=find_bdict(dict->parent, "length")) != NULL) {
					len = tmp->val.b_int;
				}
				else {
					fprintf(stderr, "Error: couldn't find length of torrent file!\n");
					exit(-1);
				}
				/* Getting the piece length */
				if ((tmp=find_bdict(dict->parent, "piece length")) != NULL) {
					piece_len = tmp->val.b_int;
				}
				else {
					fprintf(stderr, "Error: couldn't find piece length!\n");
					exit(-1);
				}
				fprintf(output, "%d:", INTCEIL(len, piece_len)*20);
				fwrite(dict->val.val, 1, INTCEIL(len, piece_len)*20, output);
			}
			else {
				fprintf(output, "%d:%s", strlen(dict->val.val), dict->val.val);
			}
			break;
		}
	
		/* Advance to next element in dictionary/list */
		dict = dict->next;
	}

	/* 
	 * Print "e" when we're done iterating through all
	 * the elements 
     */
	fprintf(output, "e");
} 

int destroy_bdict(struct bdict* dict) {
	int ret = 0;

	if (dict->next != NULL)
		ret += destroy_bdict(dict->next);
	if (dict->vtype == BDICT && dict->val.dict != NULL)
		ret += destroy_bdict(dict->val.dict);

	free(dict);

	return ret;
}

/*
 * Note the difference between get_bdict and find_bdict.
 * get_bdict takes a path to an element in the bdict dictionary
 * as a parameter and retrieves the record, whereas find_bdict 
 * just takes a key name, and searches for it.
 */

/*
 * key_path is a list of keys, ending in the key of the desired
 * dictionary. So to get the dictionary with the "name" key in the 
 * "info" dictionary, you'd use the array { "info" , "name", NULL }
 * (key paths must be NULL terminated). To get a list member, 
 * two (char*)'s are needed. The first one should be ~0 (0xFFFFFFFF
 * on 32-bit platforms) and the second should be the index into the list,
 * both typecast as (char*)'s.
 */
struct bdict* get_bdict(struct bdict* root, char** key_path) {
	unsigned long i, n;

	while (*key_path != NULL) {
		root = root->val.dict;
		if (*key_path == (char*)~0) {
			n = (unsigned long)*(++key_path);
			for (i = 0; i < n; i++)
				if ((root=root->next) == NULL)
					return NULL;
		}
		else
			do {
				if (root == NULL)
					return NULL;
				else if (strcmp(*key_path, root->key) == 0)
					break;
			} while (root = root->next);
		key_path++;
	}

	return root;
}

/*
 * strcmp is used in this function so make sure that key_name
 * is null-terminated
 */

struct bdict* find_bdict(struct bdict* root, const char* key_name) {
	struct bdict* ret;

	if (root->key != NULL && strcmp(root->key, key_name) == 0)
		return root;
	else if (root->next != NULL && (ret=find_bdict(root->next, key_name)) != NULL)
		return ret;
	else if (root->vtype == BDICT && root->val.dict != NULL
			&& (ret=find_bdict(root->val.dict, key_name)) != NULL)
		return ret;
	else
		return NULL;
}
