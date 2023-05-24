#include "Directory.h"

#include <string.h>
#include <stdlib.h>

Directory* create_directory(const char* name) {

	Directory* dir = malloc(sizeof(Directory));

	dir->parent.data = NULL;

	cstring_set(&dir->parent, "");
	dir->create_date = dir->update_date = time_now();
	dir->count_elements = 0;
	strcpy(dir->name, name);
	
	return dir;
}

int dir_is_child(Directory* dir1, Directory* dir2) {

	int k = dir2->parent.size - 1;

	while(k >= 0 && dir2->parent.data[k] != '/')
		--k;

	if(strcmp(dir2->parent.data + k + 1, dir1->name))
		return 0;
	
	dir2->parent.data[k] = '\0';
	
	if(strcmp(dir2->parent.data, dir1->parent.data)) {

		dir2->parent.data[k] = '/';
		return 0;
	}

	dir2->parent.data[k] = '/';

	return 1;
}

int file_is_child(Directory* dir, File* file) {

	int k = file->parent.size - 1;

	while(k >= 0 && file->parent.data[k] != '/')
		--k;

	if(strcmp(file->parent.data + k + 1, dir->name))
		return 0;
	
	file->parent.data[k] = '\0';
	
	if(strcmp(file->parent.data, dir->parent.data)) {

		file->parent.data[k] = '/';
		return 0;
	}

	file->parent.data[k] = '/';

	return 1;
}

int dir_is_parent(Directory* parentdir, Directory* childdir) {

	cstring s1 = {0, 0}, s2 = {0, 0};

	cstring_copy(&s1, parentdir->parent);
	cstring_append(&s1, "/");
	cstring_append(&s1, parentdir->name);

	cstring_copy(&s2, childdir->parent);
	cstring_append(&s2, "/");
	cstring_append(&s2, childdir->name);

	if(strncmp(s1.data, s2.data, s1.size)) {

		free(s1.data);
		free(s2.data);

		return 0;
	}

	free(s1.data);
	free(s2.data);

	return 1;
}
