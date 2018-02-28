#ifndef _HTTP_H
#define _HTTP_H

#include "bencode.h"

// char* zt_user_agent = "ztorrent (still in development)";

void url_encode(char* string, char* url_string);
int is_char_urlvalid(char c);
void get_url_enc_info_hash(struct bdict* torrent, char* dst);
void send_start_msg(struct bdict* torrent);
void get_hostname_from_url(char* url);



#endif
