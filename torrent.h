#ifndef _TORRENT_H
#define _TORRENT_H
#include <curl/curl.h>
#include <stdint.h>

CURLM *mhandle;

typedef struct bt_peer {
	int sockfd;
	char port[5];
	char state;
} bt_peer_t;

typedef struct torrent {
	struct bdict* root_dict;
	CURL* handle; 
	char *announce;
	uint16_t port;
	char url_info_hash[61];
	char url_peer_id[61];
	unsigned int downloaded;
	unsigned int uploaded;
	unsigned int left;
	char state;
	bt_peer_t peers[51];
	char *status;
} torrent_t;

int is_char_url_valid(char c);
char* get_announce_url(struct bdict* root);
struct torrent* start_torrent(const char* filename);
int url_encode(char *dest, const char *src, size_t n);
struct bdict* get_info_bdict(struct bdict* root);
size_t write_tracker_response(char *ptr, size_t size,
				size_t nitems, void *userdata);


#endif
