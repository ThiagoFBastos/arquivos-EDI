#include "File.h"

#include <stdlib.h>
#include <string.h>

File* create_file(const char* filename, char type) {

	File *file = malloc(sizeof(File));

	strcpy(file->name, filename);
	file->parent.data = NULL;

	cstring_set(&file->parent, "");
	
	file->type = type;
	file->size = 0;
	file->update_date = file->create_date = time_now();

	return file;
}
