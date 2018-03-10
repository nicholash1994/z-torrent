#ifndef _MACROS_H
#define _MACROS_H

// malloc with error checking
#define MALLOC(p, T, size) if ((p=(T*)malloc(size*sizeof(T))) == NULL) \
					 { fprintf(stderr, "Error: memory couldn't" \
						" be dynamically allocated!\n"); exit(-1); }

#define CLEAR(p, T, size) memset(p, 0, size*sizeof(T))

#endif
