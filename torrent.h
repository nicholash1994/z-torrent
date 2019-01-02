#ifndef _TORRENT_H
#define _TORRENT_H
#include <curl/curl.h>
#include <stdint.h>

/* Curl object handle */
CURLM *mhandle;

/* 
 * Per the name of the struct, this struct is 
 * used to store information about bittorrent 
 * peers
 */
typedef struct bt_peer {
	int sockfd;
	char port[5];
	char state;
} bt_peer_t;

/* 
 * This struct just contains all the information
 * obtained from a torrent file and the tracker. 
 * The root_dict field is the bencoded dictionary.
 * All the other field contain information about the 
 * connection (such as the port number).
 */
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

/* 
 * For more information about these functions, 
 * check out torrent.c
 */
struct torrent* start_torrent(const char* filename);
int is_char_url_valid(char c);
char* get_announce_url(struct bdict* root);
struct torrent* start_torrent(const char* filename);
int url_encode(char *dest, const char *src, size_t n);
struct bdict* get_info_bdict(struct bdict* root);
size_t write_tracker_response(char *ptr, size_t size,
				size_t nitems, void *userdata);


#endif
