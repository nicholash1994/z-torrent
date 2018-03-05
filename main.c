#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "bencode.h"
#include "error.h"
#include <rhash.h>
#include <curl/curl.h>

CURLM *mhandle;

int main(int argc, char** argv) {
	struct bdict *torrent, *info;
	char* key_path[] = {
		"info",
		(char*)NULL };

	if (curl_global_init(CURL_GLOBAL_ALL) != 0)
		err(-ZT_CONN, "Error: couldn't initialize libcurl!\n");
	if ((mhandle = curl_multi_init()) == NULL)
		err(-ZT_CONN, "Error: couldn't create CURL multi-handle!\n");

	if (argc < 2) {
		fprintf(stderr, "Error: file not specified\n");
		return -1;
	}
	
	torrent = read_torrent_file(argv[1]);
	print_bdict(torrent);

	info = get_bdict(torrent, key_path);
	print_bdict(info);

	printf("%d\n", destroy_bdict(torrent));
	
	return 0;
} 

