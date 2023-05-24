#ifndef CSTRING_H
#define CSTRING_H

#include <stdio.h>

typedef struct {

	char *data;
	size_t size;

} cstring;

int readLine(FILE*, cstring*);
void cstring_copy(cstring*, cstring);
void cstring_append(cstring*, const char*);
void cstring_add(cstring*, cstring);
void cstring_set(cstring*, const char*);

#endif
