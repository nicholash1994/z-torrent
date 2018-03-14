#ifndef _MACROS_H
#define _MACROS_H

#include <stdlib.h>
#include <string.h>

// malloc with error checking
#define MALLOC(p, T, size) if ((p=(T*)malloc(size*sizeof(T))) == NULL) \
					 { fprintf(stderr, "Error: memory couldn't" \
						" be dynamically allocated!\n"); exit(-1); }

#define CLEAR(p, T, size) memset(p, 0, size*sizeof(T))

#define strcatf(str, format, ...) \
			sprintf(str+strlen(str), format, __VA_ARGS__)

#define strncatf(str, size, format, ...) \
			snprintf(str+strlen(str), size, format, __VA_ARGS__)

#endif
