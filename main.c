#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "bencode.h"

int main(int argc, char** argv) {
	struct bdict* dict;
	char* filename;

	if (argc > 1)
		filename = argv[1];
	else {
		fprintf(stderr, "Error: need a torrent file!\n");
		return -1;
	}
	dict = read_torrent_file(filename);
		

	print_bdict(dict);
	return 0;
} 


