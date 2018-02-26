#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "bencode.h"


// returns NULL if the torrent file
// is improperly formatted
struct bdict* read_torrent_file(const char* filename) {
	FILE* torrent;
	char uc;
	char key_name[30];
	int r_depth;
	struct bdict* root_dict;

	torrent = fopen(filename, "rb");
	if (torrent == NULL) {
		fprintf(stderr, "Error: torrent file couldn't be read!\n");
		exit(-1);
	}
	
	root_dict = (struct bdict*)malloc(sizeof(struct bdict));

	r_depth = 0;
	// this parses the dictionary
	parser_ctrl(root_dict, torrent, &r_depth);

	fclose(torrent);

	return root_dict;

}

// when a special character is encountered, (e.g. 'd', 'e', 'l')
// this function decides what to do next
void parser_ctrl(struct bdict* curr, FILE* torrent, int* r_depth) {
	char uc;

	switch (uc=fgetc(torrent)) {
		case 'd':
			(*r_depth)++;
			curr->vtype = BDICT;
			read_dict(curr, torrent, r_depth);
			break;
		case 'l':
			(*r_depth)++;
			curr->vtype = BDICT;
			read_list(curr, torrent, r_depth);
			break;
		case 'i':
			(*r_depth)++;
			curr->vtype = BINT;
			read_int(curr, torrent, r_depth);
			break;
		case 'e':
			(*r_depth)--;
			break;
		default:
			return;
	}
}

// This function reads a bencoded dictionary. Since entries in bencoded
// dictionaries can also have dictionaries as their value, this function
// plays a critical role in parsing torrent files.
void read_dict(struct bdict* dict, FILE* file, int* r_depth) {
	char uc;
	int depth; // the recursion depth of the dictionary
			   // that this function is parsing

	// storing the recursion depth of the dictionary
	// that's being read
	depth = *r_depth;
	
	// creating a dictionary as the value of the current dictionary
	// and then navigating to it
	dict->val.dict = (struct bdict*)malloc(sizeof(struct bdict));
	// setting the previous dictionary (the one passed as
	// an argument to this function) as the parent of the newly
	// created dictionary
	dict->val.dict->parent = dict;
	dict = dict->val.dict;
	dict->vtype = BDICT;

	// setting the key
	dict->key = read_elem(file);

	while (1) {
		// reading the next character
		PEEK_AHEAD(uc, file);
		
		// checking to see whether the value is a UTF-8 string
		// or another datatype which is to be read by the parser
		if (uc >= '0' && uc <= '9') {
			dict->vtype = USTRING;
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
			
		PEEK_AHEAD(uc, file);
		if (uc != 'e') {
			dict->next = (struct bdict*)malloc(sizeof(struct bdict));
			dict->next->parent = dict->parent;
			dict = dict->next;
			dict->key = read_elem(file);
		}
		else
			parser_ctrl(dict, file, r_depth);
	}
}

// TODO: rewrite this function. It could be a lot more concise
// and much less complicated
void read_list(struct bdict* dict, FILE* file, int* r_depth) {
	char uc;
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

	// the only difference between a list and a dictionary
	// (in terms of how they're implemented in this program)
	// is that a list is simply a dictionary with it's entries' keys
	// set to NULL.
	dict->key = NULL;

	while (1) {
		// reading the next character
		PEEK_AHEAD(uc, file);

		// checking to see whether the value is a UTF-8 string
		// or another datatype which is to be read by the parser
		if (uc >= '0' && uc <= '9') {
			dict->vtype = USTRING;
			(dict->val).val = read_elem(file);
		}
		else {
			// parser control will also determine the type of 
			// record dict is
			parser_ctrl(dict, file, r_depth);
		}

		// if the last character which was read was 'e', then
		// *r_depth will be less than depth once parser_ctrl
		// returns, which allows us to exit the loop (and the function)
		if (depth > *r_depth) {
			dict->next = NULL;
			// considered putting a break here instead of 
			// a return
			return;
		}

		// if there are more elements in the dictionary that
		// need to be read, the following code just creates the
		// next node in the dictionary, and navigates to it
		PEEK_AHEAD(uc, file);
		if (uc != 'e') {
			dict->next = (struct bdict*)malloc(sizeof(struct bdict));
			dict->next->parent = dict->parent;
			dict = dict->next;
			dict->key = NULL;
		}
		else
			dict->next = NULL;
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
	char uc;
	char size_string[30];
	char* elem;
	unsigned long size;
	
	// this section of code simply copies the length of the element
	// into a buffer
	i = 0;
	while ((uc=fgetc(file)) != ':' && i < 30)
		size_string[i++] = uc;
	size_string[i] = '\0';

	// this just converts the size_string to an actual integer
	// and then allocates a string to store the actual element
	size = strtoul(size_string, NULL, 10);
	elem = (char*)malloc((1+size)*sizeof(char));

	for (i = 0; i < size; i++)
		elem[i] = fgetc(file);
	elem[size] = '\0';

	return elem;
}

void read_int(struct bdict* dict, FILE* file, int* r_depth) {
	char c;
	int i;
	char buffer[100];

	i = 0;
	while ((c=fgetc(file)) != 'e' && i < 99)
		buffer[i++] = c;
	buffer[i] = '\0';
	
	dict->vtype = BINT;
	dict->val.b_int = strtol(buffer, NULL, 10);

	(*r_depth)--;
}

void print_bdict(struct bdict* dict) {
	print_bdict_h(dict->val.dict, 0);
}
	
void print_bdict_h(struct bdict* dict, int depth) {
	int i;
	
	while (dict != NULL) {
		for (i = 0; i < depth; i++)
			printf(" ");

		// print key name
		if (dict->key == NULL)
			printf("*LIST*: ");
		else
			printf("%s: ", dict->key);

		// print value
		switch (dict->vtype) {
		case BINT:
			for (i = 0; i < depth; i++)
				printf(" ");
			printf("%d\n", dict->val.b_int);
			break;
		case USTRING:
			for (i = 0; i < depth; i++)
				printf(" ");
			if (dict->key != NULL && !strncmp(dict->key, "pieces", 6))
				printf("*binary data*\n");
			else
				printf("%s\n", dict->val.val);
			break;
		case BDICT:
			printf("\n");
			print_bdict_h(dict->val.dict, depth+2);
			break;
		}
		dict=dict->next;
	}
}

void print_record(struct bdict* dict) {
	if (dict->key == NULL)
		printf("Key: NULL\n");
	else
		printf("Key: %s\n", dict->key);

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


char* get_announce_url(struct bdict* dict) {
	dict = dict->val.dict;

	while (dict != NULL) {
		if (strncmp(dict->key, "announce", 8) == 0)
			return dict->val.val;
		else
			dict = dict->next;
	}

	return NULL;
}

void encode_bdict(struct bdict* dict, FILE* output) {
	bdict_stack_t stack;
	init_bdict_stack(&stack, 256);
	// these variables are needed in order to determine
	// how long the string stored in the "pieces" record is
	int len;
	int piece_len;
	
	struct bdict* tmp;
	
	dict = dict->val.dict;
	
	// getting len and piece_len
	tmp = dict;
	while (dict != NULL) {
		if (strcmp("length", dict->key) == 0)
			len = dict->val.b_int;
		else if (strcmp("piece length", dict->key) == 0)
			piece_len = dict->val.b_int;
		dict = dict->next;
	}
	dict = tmp;
	
	fprintf(output, "d");
	
	while (dict != NULL) {
		// printing the current record
		switch (dict->vtype) {
		case USTRING:
			if (dict->key != NULL) {
				fprintf(output, "%lu:", strlen(dict->key));
				fwrite(dict->key, 1, strlen(dict->key), output);

			}
			if (dict->key != NULL && strncmp(dict->key, "pieces", 6) == 0) {
				fprintf(output, "%d:", ((len/piece_len)+1)*20);
				fwrite(dict->val.val, 1, ((len/piece_len)+1)*20, output);
			}
			else {
				fprintf(output, "%lu:", strlen(dict->val.val));
				fwrite(dict->val.val, 1, strlen(dict->val.val), output);
			}
			break;
		case BINT:
			if (dict->key != NULL) {
				fprintf(output, "%lu:", strlen(dict->key));
				fwrite(dict->key, 1, strlen(dict->key), output);
				if (strncmp(dict->key, "length", 6) == 0)
					len = dict->val.b_int;
				if (strncmp(dict->key, "piece length", 12) == 0)
					piece_len = dict->val.b_int;
			}

			fprintf(output, "i%de", dict->val.b_int);
			break;
		case BDICT:
			if (dict->key != NULL) {
				fprintf(output, "%lu:", strlen(dict->key));
				fwrite(dict->key, 1, strlen(dict->key), output);
			}	
			push_bdict(&stack, dict);
			dict = dict->val.dict;
			if (dict->key == NULL)
				fprintf(output, "l");
			else
				fprintf(output, "d");
			continue;
		}

		// if dict doesn't have a record following it, either
		// navigate to the parent dictionary, or, if there is none,
		// set dict to dict->next (which would be NULL in this case)
		// which will cause the loop to finish executing
		while (dict->next == NULL) {
			if (stack.size > 0) {
				fputc('e', output);
				dict = pop_bdict(&stack);
			}
			else {
				fprintf(output, "e");
				break;
			}
		}
		dict = dict->next;
	}
}

void init_bdict_stack(bdict_stack_t* stack, int block_size) {
	stack->stack = NULL;
	stack->block_size = block_size;
	stack->size = 0;
}

void destroy_bdict_stack(bdict_stack_t* stack) {
	if (stack->stack != NULL)
		free(stack->stack);
}

void push_bdict(bdict_stack_t* stack, struct bdict* val) {
	if (stack->size%stack->block_size == 0)
		// stack->stack will be NULL if this is the first element
		// that's being pushed onto the list
		if (stack->stack == NULL)
			stack->stack = 
				(struct bdict**)malloc(
						stack->block_size*sizeof(struct bdict*));
		else
			stack->stack = (struct bdict**)realloc(stack->stack,
				(stack->size+stack->block_size)*sizeof(struct bdict*));
	stack->stack[stack->size++] = val;
}

struct bdict* pop_bdict(bdict_stack_t* stack) {
	return stack->stack[--stack->size];
}
