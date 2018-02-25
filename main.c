#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "bencode.h"

int main(int argc, char** argv) {
	struct bdict* dict;
	char* filename;
	int i, tracker_fd;
	char* announce_url;
	FILE* tracker_fp;
	FILE* tracker_res;
	FILE* ubuntu_info;
	struct addrinfo hints, tracker_addr;
	struct addrinfo* res;
	struct hostent* host;

	dict = read_torrent_file("./ubuntu.torrent");
	dict = dict->val.dict;
	while (strncmp(dict->key, "info", 4) != 0)
		dict = dict->next;

	ubuntu_info = fopen("ubuntu_info", "wb");
	encode_bdict(dict, ubuntu_info);
	fclose(ubuntu_info);
	dict = dict->parent;
	
	announce_url = get_announce_url(dict);
	if (announce_url == NULL) {
		fprintf(stderr, "Error: announce url not found");
		return -1;
	}
	announce_url="torrent.ubuntu.com";
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	i = getaddrinfo(announce_url, "6969", NULL, &res);

	if (i != 0) {
		fprintf(stderr, "Error: %s\n", gai_strerror(i));
		exit(-1);
	}

	if (res == NULL) {
		fprintf(stderr, "Error: couldn't connect to tracker!\n");
		return -1;
	}
	tracker_addr = res[0];
	
	tracker_fp = fdopen(tracker_fd=socket(AF_INET, SOCK_STREAM, 0), "ab");
	
	

	return 0;
} 

