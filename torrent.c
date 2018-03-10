#include "bencode.h"
#include "error.h"
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <rhash.h>
#include <time.h>
#include "macros.h"
#include "torrent.h"


struct torrent* start_torrent(const char* filename) {
	FILE* tmp;
	char tmp_name[] = "/tmp/zt_infoXXXXXX";
	struct torrent *t;
	struct bdict *dict;
	char *url;
	char hash[20];
	char peer_id[20];
	char *get_url;
	int i, j;
	pthread_t listen;

	MALLOC(t, struct torrent, 1);

	// getting the root dictionary
	if ((t->root_dict = read_torrent_file(filename)) == NULL)
		err(NULL, "Error: torrent file couldn't be parsed!\n");

	// initializing CURL handle
	if ((t->handle = curl_easy_init()) == NULL)
		err(NULL, "Error: curl handle couldn't be created!\n");

	// getting the announce url
	t->announce = get_announce_url(t->root_dict);

	// getting the info hash
	tmp = fdopen(mkstemp(tmp_name), "wb");
	encode_bdict(get_info_bdict(t->root_dict), tmp);
	rhash_file(RHASH_SHA1, tmp_name, hash);
	url_encode(t->url_info_hash, hash, 20);
	fclose(tmp);

	// generating the peer id
	srand(time(NULL));
	for (i = 0; i < 20; i++)
		peer_id[i] = rand()%0x100;
	url_encode(t->url_peer_id, peer_id, 20);

	// setting the event to started
	t->status = ZT_STARTED;

	return t;
}

void send_get_msg(struct torrent* t) {
	char msg[strlen(t->announce)+200];
	strcpy(msg, t->announce);
	strcpy(msg, "?info_hash=");
	strcpy(msg, t->url_info_hash);
	strcpy(msg, "&peer_id=");
	strcpy(msg, t->url_peer_id);
}

// n is the size of src
int url_encode(char *dest, const char *src, size_t n) {
	int i, j, k, len;
	
	i = j = k = 0;
	while (i < n) {
		if (is_char_url_valid(src[i]))
			dest[j++] = src[i++];
		else {
			dest[j++] = '%';
			dest[j++] = 
				(k=src[i]>>4) < 10 ? k + '0' : k-10+'A';
			dest[j++] = 
				(k=src[i++]&0x0F) < 10 ? k + '0' : k-10+'A';
		}
	}
	dest[j] = '\0';
	
	return j;
}

int is_char_url_valid(char c) {
	if ((c >= '0' && c <= '9') ||
				(c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') ||
				c == '-' || c == '_' || c == '.'
				|| c == '~')
		return 1;
	else
		return 0;
}

char* get_announce_url(struct bdict* root) {
	return find_bdict(root, "announce")->val.val;
}

struct bdict* get_info_bdict(struct bdict* root) {
	char* key_path[] = { "info", NULL };

	return get_bdict(root, key_path);
}
