#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bencode.h"
#include "http.h"
#include "error.h"
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <rhash.h>

int send_start_msg(struct bdict* torrent) {
	char msg[1024];
	char url_enc_hash[61];
	char hostname[100];
	char peer_id[20];
	struct addrinfo hints, *res;
	struct bdict* dict;
	int i;

	i = 0;

	// getting the hostname
	dict = torrent->val.dict;
	while (dict != NULL) {
		if (dict->vtype == USTRING && strcmp(dict->key, "announce") == 0)
			break;
		dict = dict->next;
	}
	if (dict == NULL) {
		err("Error: announce url not found!\n");
		return -ZT_NOFILE;
	}
	if (strlen(dict->val.val) >= 99) {
		err("Error: url is too large!\n");
		return -ZT_KEYNF;
	}
		
	strcpy(hostname, dict->val.val);
	get_hostname_from_url(hostname);
	
	// getting the info hash
	get_url_enc_info_hash(torrent, url_enc_hash);

	// generating the peer id
	srand(time(NULL));
	for (i = 0; i < 20; i++)
		peer_id[i] = rand()%0x100;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(hostname, "http", &hints, &res);

}

void get_hostname_from_url(char* url) {
	int i; // index into url
	int j;

	for (i = 7; i < strlen(url); i++)
		if (url[i] == '/' || url[i] == ':') {
			url[i-7] = '\0';
			break;
		}
		else
			url[i-7] = url[i];
}

// dst must be at least 61 bytes
void get_url_enc_info_hash(struct bdict* torrent, char* dst) {
	FILE* info_file;
	char hash[20];
	

	torrent = torrent->val.dict;
	while (strncmp(torrent->key, "info", 4) != 0)
		torrent = torrent->next;			

	info_file = fopen("/tmp/ztorrent.info", "wb");
	encode_bdict(torrent, info_file);
	fclose(info_file);

	rhash_file(RHASH_SHA1, "/tmp/ztorrent.info", hash);
	url_encode(hash,dst);

}

// url_string needs to be 3 times as large as string with one 
// extra byte for the null character
void url_encode(char* string, char* url_string) {
	int len;
	int i, j, k;

	i = j = 0;
	len = strlen(string);
	while (i < len)
		if (is_char_urlvalid(string[i]))
			url_string[j++] = string[i++];
		else { 
			url_string[j++] = '%';
			url_string[j++] = 
				(k=(string[i]&0xF0)>>4) < 10 ? k+'0' : (k-10)+'A'; 
			url_string[j++] =
				(k=string[i++]&0x0F) < 10 ? k+'0' : (k-10)+'A';
		}
	url_string[j] = '\0';
	
}

int is_char_urlvalid(char c) {
	if ((c >= '0' && c <= '9') ||
			(c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			c == '.' || c == '-' || c == '_' || c == '~')
		return 1;
	else
		return 0;
}
