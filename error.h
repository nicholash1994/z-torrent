/* This header file just contains error codes that will be used
 * in the program. 
 */
#ifndef _ZT_ERROR_H
#define _ZT_ERROR_H

#define err(...) fprintf(stderr, __VA_ARGS__)

// used if a file can't be loaded
#define ZT_NOFILE 1

// used if a torrent file contais data that's
// too large, or if a user inputs data that's
// too large
#define ZT_LONG 2

// used if a key can't be found in a bencoded dictionary
// (or any dictionary)
#define ZT_KEYNF 3

// used for connection errors
#define ZT_CONN 4


#endif
