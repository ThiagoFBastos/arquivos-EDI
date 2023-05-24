#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "Date.h"
#include "File.h"

typedef struct Directory {

	char name[FILENAME_MAX];
	cstring parent;
	int count_elements;
	struct Date update_date;
	struct Date create_date;

} Directory;

Directory* create_directory(const char*);

int dir_is_child(Directory*, Directory*);
int file_is_child(Directory*, File*);

int dir_is_parent(Directory*, Directory*);

#endif
