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
	struct bdict *torrent;

	if (argc < 2) {
		fprintf(stderr, "Error: file not specified");
		return -1;
	}
	
	torrent = read_torrent_file(argv[1]);

	send_start_msg(torrent);
	
	return 0;
} 

