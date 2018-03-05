#include "bencode.h"
#include "error.h"
#include <string.h>
#include <curl/curl.h>
#include <rhash.h>
#include "torrent.h"

enum event_stat { ZT_STARTED, ZT_COMPLETED, ZT_STOPPED };

struct torrent {
	CURL* handle;
	struct bdict* root_dict;
	char url_info_hash[61];
	char url_peer_id[61];
	unsigned int downloaded;
	unsigned int uploaded;
	unsigned int left;
	char flags;
	FILE **peers;
	enum event_stat status;
};

struct torrent* start_torrent(const char* filename) {
	struct torrent *t;
	struct bdict *dict;
	char *url;
	char get_url[300];
	int i, j;

	t = (struct torrent*)malloc(sizeof(struct torrent));

	// getting the root dictionary
	if ((t->root_dict = read_torrent_file(filename)) == NULL)
		err(NULL, "Error: torrent file couldn't be parsed!\n");

	if ((t->handle = curl_easy_init()) == NULL)
		err(NULL, "Error: curl handle couldn't be created!\n");

	// getting the announce url
	dict = t->root_dict->val.dict;
	url = NULL;
	while (dict != NULL)
		if (strcmp(dict->key, "announce") == 0)
			url = dict->key;
	if (url == NULL)
		err(NULL, "Error: couldn't find announce URL ",
							"in torrent file!\n");
	
	// getting the info hash

	i = 0;
	strcpy(get_url, url, j=strlen(url));i+=j;
	url_encode(t->url_info_hash,
		rhash_
	





}

int url_encode(char *dest, const char *src) {
	int i, j, k, len;
	
	i = j = k = 0;
	len = strlen(src);
	while (i < len) {
		if (is_char_url_valid(src[i]))
			dest[j++] = src[i++];
		else {
			dest[j++] = '%';
			dest[j++] = 
				(k=(src[i]&0xF0)>>4) < 10 ? k + '0' : k-10+'A';
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
	return find_bdict(root, "announce");
}

struct bdict* get_info_bdict(struct bdict* root) {
	char key_path[] = { "info", NULL };

	return get_bdict(root, key_path);
}
