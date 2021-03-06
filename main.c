#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "bencode.h"
#include "torrent.h" 
#include "error.h"
#include <rhash.h>
#include <curl/curl.h>

int main(int argc, char** argv) {
	struct torrent *torrent;
	struct bdict* dict;
	char *torrent_path = NULL;
	int i;
	
	if (curl_global_init(CURL_GLOBAL_ALL) != 0)
		err(-ZT_CONN, "Error: couldn't initialize libcurl!\n");
	if ((mhandle = curl_multi_init()) == NULL)
		err(-ZT_CONN, "Error: couldn't create CURL multi-handle!\n");

	if (argc < 2) {
		fprintf(stderr, "Error: file not specified\n");
		return -1;
	}

	/* Processing command-line arguments */
	for (i = 0; argv[i] != NULL; i++) {
		if (strcmp(argv[i], "-f") == 0 && argv[i+1] != NULL)
			torrent_path = argv[i+1];
	}
	if (torrent_path == NULL) {
		fprintf(stderr, "Error: file not specified!\n");
		return -1;
	}

	torrent = start_torrent(torrent_path);
	/*
	dict = read_torrent_file(torrent_path);
	encode_bdict(dict, stdout);
	destroy_bdict(dict);
	*/

	return 0;
} 

