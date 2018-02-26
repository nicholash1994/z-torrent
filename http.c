#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bencode.h"
#include "http.h"
#include <rhash.h>

// dst must be at least 61 bytes
void get_url_enc_info_hash(struct bdict* torrent, char* dst) {
	FILE* info_file;
	char hash[20];
	

	torrent = read_torrent_file("ubuntu.torrent");
	torrent = torrent->val.dict;
	while (strncmp(torrent->key, "info", 4) != 0)
		torrent = torrent->next;
 
/*
	torrent = torrent->val.dict;
	b[0] = torrent;
	b[1] = torrent->next;
	b[2] = torrent->next->next;
	b[3] = torrent->next->next->next;
	
	for (i = 0; i < 4; i++) {
		min = b[k=i];
		for (j = i+1; j < 4; j++) {
			if (strcmp(b[j]->key, min->key) < 0)
				min = b[k=j];
		}
		torrent = b[i];
		b[i] = b[k];
		b[k] = torrent;
	}
	for (i = 0; i < 3; i++)
		b[i]->next = b[i+1];
	b[3]->next = NULL;
	torrent = b[0]->parent; 
	torrent->val.dict = b[0];
*/			

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
