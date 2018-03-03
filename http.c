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
char* url_regex_str = "https?://([^:/]*):?([0-9]+)?(/[^ ]*)";

struct http_conn {
	FILE* socket;
	char* hostname;
	char* resource;
	int port;
};

struct torrent {
	struct http_conn conn;
	struct bdict* root_dict;
	char url_info_hash[61];
	char url_peer_id[61];
};

int send_start_msg(struct bdict* torrent) {
	char msg[1024];
	char buf[1024];
	char url_enc_hash[61];
	char hostname[100];
	char url_resource[100];
	char service[10];
	char peer_id[20];
	char url_peer_id[61];
	struct addrinfo hints, *res;
	struct bdict* dict;
	int i, j, trackerfd;
	regex_t url_regex, regex1;
	regmatch_t results[20];
	char regex_error_buf[100];

	i = 0;

	// finding the announce url in the benecoded dictionary
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
	
	get_url_enc_info_hash(torrent, url_enc_hash);
		
	memset(results, 0, 20*sizeof(regmatch_t));
	// getting the hostname, url, and the resource
	//  with a regular expression
	regcomp(&url_regex, url_regex_str, REG_EXTENDED);
	regexec(&url_regex, dict->val.val, 20, results, 0);

	strncpy(hostname, &dict->val.val[results[1].rm_so], 
					results[1].rm_eo - results[1].rm_so);
	hostname[results[1].rm_eo - results[1].rm_so] = '\0';

	// i.e. if the url didn't specify a port number
	if (results[3].rm_so == -1) {
		strcpy(service, "80");
		strncpy(url_resource, &dict->val.val[results[2].rm_so],
					results[2].rm_eo - results[2].rm_so);
		url_resource[results[2].rm_eo - results[2].rm_so] = '\0';
	}
	// i.e. if a port number was specified
	else {
		strncpy(service, &dict->val.val[results[2].rm_so],	
					results[2].rm_eo - results[2].rm_so);
		service[results[2].rm_eo - results[2].rm_so] = '\0';
		
		strncpy(url_resource, &dict->val.val[results[3].rm_so],
					results[3].rm_eo - results[3].rm_so);
		url_resource[results[3].rm_eo - results[3].rm_so] = '\0';
	}

	// generating the peer id
	srand(time(NULL));
	for (i = 0; i < 20; i++)
		peer_id[i] = rand()%0x100;
	url_encode(peer_id, url_peer_id);
	
	// getting address info
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((i=getaddrinfo(hostname, "http", &hints, &res)) != 0) {
		err("Error: %s\n", gai_strerror(i));
		return -ZT_CONN;
	}

	// creating the socket
	if ((trackerfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		err("Error: couldn't connect to tracker!\n");
		return -ZT_CONN;
	}

	// connecting to the server
	if (connect(trackerfd, res->ai_addr, res->ai_addrlen) == -1) {
		err("Error: failed to connect to the server!\n");
		return -ZT_CONN;
	}
	
	// crafting the HTTP GET request
	// strncpy is used to avoid unnecessarily copying the null byte

	// the expression (-i)+(i+=x) increments i by x and evaluates
	// to (or returns) x
	i = 0;

	craft_get_request(msg, url_resource, url_enc_hash, url_peer_id,
			service, hostname);
	
	printf("%s\n\n", msg);

	// sending the message
	if (send(trackerfd, msg, 1024, 0) == -1) {
		err("Error: failed to send initial message to tracker!\n");
		freeaddrinfo(res);
		return -ZT_CONN;
	}

	// this regex is for in case a 301 response (redirect response) is
	// received
	regcomp(&regex1, "Location: https?://([^/:]+):?([0-9]+)?(/.*)$",
				REG_EXTENDED);

	while ((i=recv(trackerfd, buf, 1024, 0)) != EOF && i != -1) {
		if (strncmp(msg, "HTTP/1.1 301", 12) == 0) {
			regexec(&regex1, msg, 20, results, 0);
			
		}
		if (i != 0)
			fwrite(buf, 1, i, stdout);
	}

}

// all strings and arrays must be null-terminated
int craft_get_request(FILE* socket, const char* url_resource, 
			const char** keys, const char** headers) {

	strncpy(msg, url_resource
}

/*
// returns the length of msg
int craft_get_request(char* msg, const char* url_resource, 
				const char* url_enc_hash, const char* url_peer_id, 
				const char* service, const char* hostname) {
	int i, j;

	i = 0;
	strncpy(&msg[i], "GET ", 4);i+=4;
	strncpy(&msg[i], url_resource, j=strlen(url_resource));i+=j;
	strncpy(&msg[i], "?info_hash=", 11);i+=11;
	strncpy(&msg[i], url_enc_hash, j=strlen(url_enc_hash));i+=j;
	strncpy(&msg[i], "&peer_id=", 9);i+=9;
	strncpy(&msg[i], url_peer_id, j=strlen(url_peer_id));i+=j;
	strncpy(&msg[i], "&port=", 6);i+=6;
	strncpy(&msg[i], service, j=strlen(service));i+=j;
	strncpy(&msg[i], "&uploaded=0", 11);i+=11;
	strncpy(&msg[i], "&downloaded=0", 13);i+=13;
	strncpy(&msg[i], "&left=1502576640", 16);i+=16;
	strncpy(&msg[i], "&event=started", 14);i+=14;
	strncpy(&msg[i], " HTTP/1.1\r\nHost: ", 17);i+=17;
	strncpy(&msg[i], hostname, j=strlen(hostname));i+=j;
	strncpy(&msg[i], "\r\n\r\n", 4);i+=4;
	msg[i] = '\0';
	
	return i;

}
*/

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
// the first argument must be null-terminated
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
				(k=(string[i]&0xF0)>>4) < 10 ? k+'0' : (k-10)+'a'; 
			url_string[j++] =
				(k=string[i++]&0x0F) < 10 ? k+'0' : (k-10)+'a';
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
