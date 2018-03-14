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
#include "ports.h"


int main(int argc, char** argv) {
	struct torrent *torrent;
	
	if (curl_global_init(CURL_GLOBAL_ALL) != 0)
		err(-ZT_CONN, "Error: couldn't initialize libcurl!\n");
	if ((mhandle = curl_multi_init()) == NULL)
		err(-ZT_CONN, "Error: couldn't create CURL multi-handle!\n");

	if (argc < 2) {
		fprintf(stderr, "Error: file not specified\n");
		return -1;
	}

	init_ports();
	
	torrent = start_torrent(argv[1]);

	return 0;
} 

