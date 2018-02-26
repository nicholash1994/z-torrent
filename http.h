#ifndef _HTTP_H
#define _HTTP_H

#include "bencode.h"

void url_encode(char* string, char* url_string);
int is_char_urlvalid(char c);
void get_url_enc_info_hash(struct bdict* torrent, char* dst);

#endif
