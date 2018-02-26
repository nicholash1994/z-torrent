#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "bencode.h"
#include "http.h"
#include <rhash.h>

int main(int argc, char** argv) {
	struct bdict* torrent;
	char* filename;
	int i, tracker_fd;
	char* announce_url;
	FILE* tracker_fp;
	FILE* tracker_res;
	FILE* my_file_pointer;
	struct addrinfo hints, tracker_addr;
	struct addrinfo* res;
	struct hostent* host;
	
	char url_hash[61];

	get_url_enc_info_hash(torrent, url_hash);
	printf("%s\n\n", url_hash);
	
	return 0;
} 

