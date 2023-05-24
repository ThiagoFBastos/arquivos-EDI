#include "NTree.h"

#include "Directory.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int remove_node(NTree* parent, NTree* srcnode) {

	NTree *node, *prev;

	node = parent->child;

	if(!node) 
		return 0;

	else if(node == srcnode) {

		parent->child = node->sibling;

		if(node->type == 'D') {

			Directory *dir = node->value;

			dispose(node->child);

			node->child = NULL;

			free(dir->parent.data);
			free(dir);
		}

		else {

			File *file = node->value;

			free(file->parent.data);
			free(file);
		}

		free(node);

		return 1;
	}

	prev = node;
	node = node->sibling;

	while(node) {

		if(node == srcnode) {

			prev->sibling = node->sibling;

			if(node->type == 'D') {

				Directory *dir = node->value;

				dispose(node->child);

				node->child = NULL;
				
				free(dir->parent.data);
				free(dir);
			}

			else {

				File *file = node->value;

				free(file->parent.data);
				free(file);
			}

			free(node);

			return 1;
		}

		prev = node;
		node = node->sibling;
	}

	return 0;
}

int is_direct_child(NTree* node, const char* name) {

	if(node->type != 'D')
		return 0;

	else {

		NTree* no;

		for(no = node->child; no; no = no->sibling) {

			if(no->type == 'D' && !strcmp(((Directory*)no->value)->name, name) || no->type == 'F' && !strcmp(((File*)no->value)->name, name))
				return 1;
		}
	}

	return 0;
}

NTree* create_node(void* value, char type) {

	NTree* no = malloc(sizeof(NTree));

	no->value = value;
	no->child = no->parent = no->sibling = NULL;
	no->type = type;

	return no;
}

void dispose(NTree* node) {

	if(node) {

		dispose(node->sibling);
		dispose(node->child);

		if(node->type == 'F')
			free(((File*)node->value)->parent.data);

		else
			free(((Directory*)node->value)->parent.data);
		
		free(node->value);
		free(node);
	}
}

int add_node(NTree* parent, void* value, char type) {

	NTree* node;
	Directory* destdir = parent->value;

	if(type == 'D') {

		Directory* dir = value;

		cstring_copy(&dir->parent, destdir->parent);
		cstring_append(&dir->parent, "/");
		cstring_append(&dir->parent, destdir->name);

		node = create_node(dir, 'D');
	}

	else {

		File* file = value;

		cstring_copy(&file->parent, destdir->parent);
		cstring_append(&file->parent, "/");
		cstring_append(&file->parent, destdir->name);

		node = create_node(file, 'F');
	}
	
	if(insert_tree(parent, node))
		return 1;

	free(node);

	return 0;
}

int insert_tree(NTree* parent, NTree* child) {

	if(!is_direct_child(parent, child->value)) {

		NTree* no = parent->child;

		child->parent = parent;
		child->sibling = no;
		parent->child = child;

		return 1;
	}

	return 0;
}
