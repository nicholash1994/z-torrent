#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bencode.h"
#include "http.h"
#include "error.h"
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <regex.h>
#include <netdb.h>
#include <rhash.h>

char* zt_user_agent = "ZTorrent 0.0.1";
char* port_regex_str = "//(?<=com://)[0-9]*";
char* hostname_regex_str = "http:\\\\([\\w\\d-]+)/{0,1}";

int send_start_msg(struct bdict* torrent) {
	char msg[1024];
	char url_enc_hash[61];
	char hostname[100];
	char peer_id[20];
	struct addrinfo hints, *res;
	struct bdict* dict;
	int i, trackerfd;
	regex_t hostname_regex, port_regex;
	regmatch_t results[20];
	char regex_error_buf[100];

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
		
	memset(results, 0, 20*sizeof(regmatch_t));
	// getting the hostname with a regular expression
	regcomp(&hostname_regex, hostname_regex_str, 0);
	regexec(&hostname_regex, dict->val.val, 10, results, 0);

	// getting the port number
	i=regcomp(&port_regex, port_regex_str, 0);
	i=regexec(&port_regex, dict->val.val, 10, &results[10], 0);
	regerror(i, &port_regex, regex_error_buf, 100);
	printf("Error: %s\n", regex_error_buf);

	strcpy(hostname, dict->val.val);
	get_hostname_from_url(hostname);
	
	// getting the info hash
	get_url_enc_info_hash(torrent, url_enc_hash);

	// generating the peer id
	srand(time(NULL));
	for (i = 0; i < 20; i++)
		peer_id[i] = rand()%0x100;
	
	// getting address info
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((i=getaddrinfo(hostname, "http", &hints, &res)) != 0) {
		err("Error: %s\n", gai_strerror(i));
		return -ZT_CONN;
	}

	if ((trackerfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		err("Error: couldn't connect to tracker!\n");
		return -ZT_CONN;
	}

	connect(trackerfd, res->ai_addr, res->ai_addrlen);
	//strcpy(msg, "GET 	

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
