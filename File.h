#ifndef FILE_H
#define FILE_H

#include "Date.h"
#include "cstring.h"

typedef struct File {

	char name[FILENAME_MAX];
	cstring parent;
	long size;
	char type;
	struct Date create_date;
	struct Date update_date;

} File;

File* create_file(const char*, char);

#endif
