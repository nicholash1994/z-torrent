#ifndef _HTTP_H
#define _HTTP_H

#include "bencode.h"

int send_start_msg(struct bdict* torrent);
void url_encode(char* string, char* url_string);
int is_char_urlvalid(char c);
void get_url_enc_info_hash(struct bdict* torrent, char* dst);
int craft_get_request(char* msg, const char* url_resource, 
				const char* url_enc_hash, const char* url_peer_id, 
				const char* service, const char* hostname);



#endif
