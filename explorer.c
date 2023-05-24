#include "Directory.h"
#include "List.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <stddef.h>

static int nomeValido(const char*);

static void __swap__(NTree*, const char*, const char*);

static void __rm__(NTree*, const char*);

static void __search__(NTree*, const char*, size_t);

static void reload(NTree*);
static void __mv__(NTree*, const char*, const char*);

static void __mkfile__(NTree*, char*, char*);
static NTree* cd(NTree*, const char*);
static NTree* getNode(NTree*, const char*);

static void __rename__(NTree*, const char*, const char*);
static void __mkdir__(NTree*, char*);
static void __ls__(NTree*, const char*);

static File* create_file_from_s(cstring);
static Directory* create_directory_from_s(cstring);
static NTree* create_ntree_from_file(const char*);

int nomeValido(const char* s) {

	while(*s) {

		if(*s == '/' || isspace(*s))
			return 0;

		++s;
	}

	return 1;
}

void __swap__(NTree* current_dir, const char* path, const char* type) {

	NTree* node;

	node = getNode(current_dir, path);

	if(node) {

		if(node->type == 'D') {

			Directory* dir = node->value;

			if(!(strcmp(type, "T") && strcmp(type, "B"))) {

				File* file = create_file(dir->name, type[0]);

				dispose(node->child);
				node->child = NULL;

				file->create_date = dir->create_date;
				cstring_copy(&file->parent, dir->parent);
				node->value = file;
				node->type = 'F';

				free(dir->parent.data);
				free(dir);
			}
		}

		else {

			File* file = node->value;

			if(!strcmp(type, "D")) {

				Directory* dir = create_directory(file->name);

				dir->create_date = file->create_date;
				cstring_copy(&dir->parent, file->parent);

				node->value = dir;
				node->type = 'D';

				free(file->parent.data);
				free(file);
			}

			else
				file->type = type[0];
		}
	}

	else
		fprintf(stderr, "o caminho %s não foi encontrado!\n", path);
}

void __rm__(NTree* current_dir, const char* path) {

	NTree* node;

	node = getNode(current_dir, path);

	if(node) {

		Directory *dir = node->parent->value;

		remove_node(node->parent, node);

		--dir->count_elements;
		dir->update_date = time_now();
	}

	else
		fprintf(stderr, "o caminho %s não foi encontrado!\n", path);
}

void __search__(NTree* node, const char* name, size_t n) {

	if(node) {

		if(node->type == 'D') {

			Directory* dir = node->value;

			if(!strncmp(name, dir->name, n)) {

				printf("%s/%s|type : D|create : %02hd/%02hd/%04hd %02hd:%02hd| updated : %02hd/%02hd/%04hd %02hd:%02hd| elements : %d\n", 

				dir->parent.data,
				dir->name,
				dir->create_date.day, 
				dir->create_date.month,
				dir->create_date.year,
				dir->create_date.hour,
				dir->create_date.minute,
				dir->update_date.day,
	  			dir->update_date.month,
				dir->update_date.year,
				dir->update_date.hour,
				dir->update_date.minute,
				dir->count_elements);
			}
		}

		else {

			File *file = node->value;

			if(!strncmp(name, file->name, n)) {

				printf("%s/%s|type : %c|create : %02hd/%02hd/%04hd %02hd:%02hd| updated : %02hd/%02hd/%04hd %02hd:%02d| size : %ld\n", 

				file->parent.data,
				file->name,			
				file->type,
				file->create_date.day, 
				file->create_date.month,
				file->create_date.year,
				file->create_date.hour,
				file->create_date.minute,
				file->update_date.day,
	  			file->update_date.month,
				file->update_date.year,
				file->update_date.hour,
				file->update_date.minute,
				file->size);
			}
		}

		__search__(node->sibling, name, n);
		__search__(node->child, name, n);
	}
}

void reload(NTree* node) {

	if(node) {

		Directory* destdir = node->parent->value;

		if(node->type == 'D') {

			Directory *dir = node->value;

			cstring_copy(&dir->parent, destdir->parent);
			cstring_append(&dir->parent, "/");
			cstring_append(&dir->parent, destdir->name);

			reload(node->child);
		}

		else {

			File* file = node->value;

			cstring_copy(&file->parent, destdir->parent);
			cstring_append(&file->parent, "/");
			cstring_append(&file->parent, destdir->name);
		}

		reload(node->sibling);
	}
}

void __mv__(NTree* current_dir, const char* path, const char* newpath) {

	NTree *node = strcmp(path, ".") ? getNode(current_dir, path) : current_dir;

	if(node) {

		NTree *destnode = strcmp(newpath, ".") ? cd(current_dir, newpath) : current_dir;

		if(!destnode || destnode->type != 'D')
			fprintf(stderr, "o caminho %s não corresponde a um diretório!\n", newpath);

		else if(node->type != 'D' || !dir_is_parent(node->value, destnode->value)){

			NTree *no, *prev, *parent;
			Directory *parentdir, *destdir;

			parent = node->parent;
			parentdir = parent->value;
			destdir = destnode->value;
			no = parent->child;

			if(no == node) {

				NTree *sibling = node->sibling;

				if(insert_tree(destnode, node)) {

					parent->child = sibling;

					--parentdir->count_elements;
					++destdir->count_elements;
					parentdir->update_date = destdir->update_date = time_now();

					if(node->type == 'D') {

						Directory* dir = node->value;

						cstring_copy(&dir->parent, destdir->parent);
						cstring_append(&dir->parent, "/");
						cstring_append(&dir->parent, destdir->name);

						reload(node->child);
					}

					else {

						File* file = node->value;

						cstring_copy(&file->parent, destdir->parent);
						cstring_append(&file->parent, "/");
						cstring_append(&file->parent, destdir->name);
					}
				}

				else
					fprintf(stderr, "%s já está sendo usado na pasta\n", path);			
			}

			else {

				prev = no;
				no = no->sibling;

				while(no) {

					if(no == node) {

						NTree *sibling = node->sibling;

						if(insert_tree(destnode, node)) {

							prev->sibling = sibling;

							--parentdir->count_elements;
							++destdir->count_elements;
							parentdir->update_date = destdir->update_date = time_now();

							if(node->type == 'D') {

								Directory* dir = node->value;

								cstring_copy(&dir->parent, destdir->parent);
								cstring_append(&dir->parent, "/");
								cstring_append(&dir->parent, destdir->name);

								reload(node->child);
							}

							else {

								File* file = node->value;


								cstring_copy(&file->parent, destdir->parent);
								cstring_append(&file->parent, "/");
								cstring_append(&file->parent, destdir->name);
							}						
						}

						else
							fprintf(stderr, "%s já está sendo usado na pasta\n", path);
	
						break;
					}

					prev = no;
					no = no->sibling;
				}
			}
		}

		else

			fprintf(stderr, "essa operacão não é permitida\n");
	}

	else	
		fprintf(stderr, "o caminho %s não foi encontrado!\n", path);
}

void __ls__(NTree* current_dir, const char* path) {

	NTree *node;

	if(path) {

		current_dir = cd(current_dir, path);

		if(!current_dir) {

			fprintf(stderr, "o caminho %s não encontrado!\n", path);
			return;
		}
	}

	if(current_dir->type != 'D') {

		fprintf(stderr, "o caminho %s não corresponde a um diretório!\n", path);
		return;
	}

	for(node = current_dir->child; node; node = node->sibling) {

		if(node->type == 'D') {

			Directory *dir = node->value;

			printf("type : D|name : %s|created : %02hd/%02hd/%04hd %02hd:%02hd| updated : %02hd/%02hd/%04hd %02hd:%02hd| elements : %d\n", 

			dir->name, 
			dir->create_date.day, 
			dir->create_date.month,
			dir->create_date.year,
			dir->create_date.hour,
			dir->create_date.minute,
			dir->update_date.day,
  			dir->update_date.month,
			dir->update_date.year,
			dir->update_date.hour,
			dir->update_date.minute,
			dir->count_elements);
		}

		else {

			File *file = node->value;

			printf("type %c|name : %s|created : %02hd/%02hd/%04hd %02hd:%02hd| updated : %02hd/%02hd/%04hd %02hd:%02hd| size : %ld\n", 

			file->type,
			file->name, 
			file->create_date.day, 
			file->create_date.month,
			file->create_date.year,
			file->create_date.hour,
			file->create_date.minute,
			file->update_date.day,
  			file->update_date.month,
			file->update_date.year,
			file->update_date.hour,
			file->update_date.minute,
			file->size);
		}
	}
}


void __mkfile__(NTree *current_dir, char *path, char *type) {

	File* file;
	char* ptr;

	if(strcmp(type, "T") && strcmp(type, "B")) {

		fprintf(stderr, "o tipo de arquivo não é suportado\n");
		return;
	} 

	ptr = strrchr(path, '/');

	if(ptr) {

		*ptr = '\0';

		current_dir = cd(current_dir, path);

		*ptr = '/';

		if(!current_dir) {

			fprintf(stderr, "o caminho %s não foi encontrado!\n", path);
			return;
		}

		++ptr;
	}

	else
		ptr = path;

	if(current_dir->type != 'D') {
	
		fprintf(stderr, "%s não é um diretório!\n", path);
		return;
	}

	file = create_file(ptr, type[0]);

	if(add_node(current_dir, file, 'F')) {

		Directory *destdir = current_dir->value;

		++destdir->count_elements;
		destdir->update_date = time_now();
	}

	else {

		free(file->parent.data);
		free(file);
		fprintf(stderr, "%s já está sendo usado no diretório!\n", ptr);
	}
}

NTree* cd(NTree* node, const char* s) {

	cstring name = {0, 0};
	char *ptr;

	if(!s)
		return NULL;

	cstring_set(&name, s);

	ptr = strtok(name.data, "/");

	while(ptr && !strcmp(ptr, "..") && node->parent) {

		ptr = strtok(NULL, "/");
		node = node->parent;
	}

	if(!ptr) {

		free(name.data);
		return node;
	}

	else if(!strcmp(ptr, "..")) {

		free(name.data);
		return NULL;
	}

	while(node && ptr) {

		NTree* no = NULL;

		for(no = node->child; no; no = no->sibling) {

			if(no->type == 'D' && !strcmp(((Directory*)no->value)->name, ptr))
				break;
		}

		node = no;
		ptr = strtok(NULL, "/");
	}

	free(name.data);

	return node;
}

NTree* getNode(NTree* node, const char* s) {

	cstring name = {0, 0};
	char *ptr;

	if(!s)
		return NULL;

	cstring_set(&name, s);

	ptr = strtok(name.data, "/");

	while(ptr && !strcmp(ptr, "..") && node->parent) {

		ptr = strtok(NULL, "/");
		node = node->parent;
	}

	if(!ptr) {

		free(name.data);
		return node;
	}

	else if(!strcmp(ptr, "..")) {

		free(name.data);
		return NULL;
	}

	while(node && ptr) {

		NTree* no = NULL;
		char *next_ptr = strtok(NULL, "/");

		if(next_ptr) {

			for(no = node->child; no; no = no->sibling) {

				if(no->type == 'D' && !strcmp(((Directory*)no->value)->name, ptr))
					break;
			}
		}

		else {

			for(no = node->child; no; no = no->sibling) {

				if(no->type == 'D' && !strcmp(((Directory*)no->value)->name, ptr) || no->type == 'F' && !strcmp(((File*)no->value)->name, ptr))
					break;
			}
		}

		node = no;
		ptr = next_ptr;
	}

	free(name.data);

	return node;
}

void __rename__(NTree* current_dir, const char* oldname, const char* newname) {


	if(nomeValido(newname)) {

		NTree* node = strcmp(oldname, ".") ? getNode(current_dir, oldname) : current_dir;

		if(node) {

			if(node->parent && is_direct_child(node->parent, newname))
				fprintf(stderr, "%s já está sendo usado no diretório\n", newname);

			else if(node->type == 'D') {

				Directory* dir = node->value;

				strcpy(dir->name, newname);
				dir->update_date = time_now();

				reload(node->child);
			}

			else {

				File* file = node->value;

				strcpy(file->name, newname);
				file->update_date = time_now();
			}
		}

		else
			fprintf(stderr, "não existe diretório ou arquivo com este nome!\n");
	}

	else
		fprintf(stderr, "o nome do conteúdo não pode conter \'/\' ou espaços!\n");
	
}

void __mkdir__(NTree* current_dir, char* path) {

	Directory* dir;
	char* ptr;

	ptr = strrchr(path, '/');

	if(ptr) {

		*ptr = '\0';

		current_dir = cd(current_dir, path);

		*ptr = '/';

		if(!current_dir) {

			fprintf(stderr, "o caminho %s não foi encontrado!\n", path);
			return;
		}

		++ptr;
	}

	else
		ptr = path;

	if(current_dir->type != 'D') {
	
		fprintf(stderr, "%s não é um diretório!\n", path);
		return;
	}

	dir = create_directory(ptr);

	if(add_node(current_dir, dir, 'D')) {

		Directory *destdir = current_dir->value;

		++destdir->count_elements;
		destdir->update_date = time_now();
	}

	else {

		free(dir->parent.data);
		free(dir);
		fprintf(stderr, "%s já está sendo usado no diretório!\n", ptr);
	}
}

int main(int argc, char* argv[]) {

	NTree* root = NULL, *current_dir = NULL;
	Directory* dir = NULL;
	cstring comando = {NULL, 0};

	if(argc > 1 && !strcmp(argv[1], "-bd"))
		root = create_ntree_from_file(argv[2]);

	else
		root = create_node(create_directory("computador"), 'D');

	dir = root->value;
	current_dir = root;

	while(1) {

		char* ptr;
		
		printf("%s/%s?> ", dir->parent.data, dir->name);

		readLine(stdin, &comando);

		if(!comando.size) {

			putchar('\n');
			continue;
		}

		ptr = strtok(comando.data, " ");

		if(!strcmp(ptr, "rename")) {

			char *oldname, *newname;

			oldname = strtok(NULL, " ");
			newname = strtok(NULL, " ");

			if(oldname && newname)
				__rename__(current_dir, oldname, newname);
			else
				system("cat options");
		}

		else if(!strcmp(ptr, "search")) {

			char *name = strtok(NULL, " ");

			if(name)
				__search__(current_dir, name, strlen(name));
			else
				system("cat options");
		}

		else if(!strcmp(ptr, "cd")) {

			NTree *newdir;
			char *path;

			path = strtok(NULL, " ");

			if(path) {

				newdir = cd(current_dir, path);

				if(newdir) {

					current_dir = newdir;
					dir = newdir->value;
				}

				else
					fprintf(stderr, "o diretório %s não foi encontrado\n", path);
			}

			else
				system("cat options");
		}

		else if(!strcmp(ptr, "mkdir")) {

			char *dirname = strtok(NULL, " ");

			if(dirname)
				__mkdir__(current_dir, dirname);
			else
				system("cat options");
		}

		else if(!strcmp(ptr, "mkfile")) {

			char *name, *type;

			name = strtok(NULL, " ");
			type = strtok(NULL, " ");

			if(name && type)
				__mkfile__(current_dir, name, type);

			else
				system("cat options");
		}

		else if(!strcmp(ptr, "rm")) {

			char *name = strtok(NULL, " ");

			if(name) {

				puts("Quer realmente eliminar este conteúdo?[y/n]");

				if(toupper(getchar()) == 'Y')
					__rm__(current_dir, name);
				else
					puts("operacão cancelada!");

				getchar();
			}

			else
				system("cat options");
		}

		else if(!strcmp(ptr, "swp")) {

			char *path, *type;

			path = strtok(NULL, " ");
			type = strtok(NULL, " ");

			if(path && type) {

				puts("Quer trocar o tipo do conteúdo?[Y/N]");

				if(toupper(getchar()) == 'Y')
					__swap__(current_dir, path, type);
				
				else
					puts("operacão cancelada!");

				getchar();
			}

			else
				system("cat options");
		}

		else if(!strcmp(ptr, "mv")) {

			char *path, *newpath;

			path = strtok(NULL, " ");
			newpath = strtok(NULL, " ");

			if(path && newpath)
				__mv__(current_dir, path, newpath);
			else
				system("cat options");
		}

		else if(!strcmp(ptr, "ls"))
			__ls__(current_dir, strtok(NULL, " "));

		else if(!strcmp(ptr, "clear"))
			system("clear");

		else if(!strcmp(ptr, "prop")) {

			char *path = strtok(NULL, " ");
			NTree *node = path ? getNode(current_dir, path) : current_dir;

			if(!node)
				fprintf(stderr, "%s não foi encontrado\n", path);

			else if(node->type == 'F') {

				File *file = node->value;

				printf("%s/%s|type : %c|create : %02hd/%02hd/%04hd %02hd:%02hd| updated : %02hd/%02hd/%04hd %02hd:%02d| size : %ld\n", 

					file->parent.data,
					file->name,			
					file->type,
					file->create_date.day, 
					file->create_date.month,
					file->create_date.year,
					file->create_date.hour,
					file->create_date.minute,
					file->update_date.day,
		  			file->update_date.month,
					file->update_date.year,
					file->update_date.hour,
					file->update_date.minute,
					file->size);
			}

			else {

				Directory *dir = node->value;

				printf("%s/%s|type : D|create : %02hd/%02hd/%04hd %02hd:%02hd| updated : %02hd/%02hd/%04hd %02hd:%02hd| elements : %d\n", 

					dir->parent.data,
					dir->name,
					dir->create_date.day, 
					dir->create_date.month,
					dir->create_date.year,
					dir->create_date.hour,
					dir->create_date.minute,
					dir->update_date.day,
		  			dir->update_date.month,
					dir->update_date.year,
					dir->update_date.hour,
					dir->update_date.minute,
					dir->count_elements);
			}
		}

		else if(!strcmp(ptr, "exit"))
			break;

		else
			system("cat options");
	}
	
	return 0;
}

File* create_file_from_s(cstring s) {

	File* file = malloc(sizeof(File));
	int i = s.size - 1;

	file->type = s.data[0];

	while(i >= 0 && s.data[i] != '/')
		--i;

	--i;

	while(i >= 0 && s.data[i] != '/')
		--i;

	if(i < 0 || sscanf(s.data + i + 1, "%hd-%hd-%hd/%hd:%hd", &file->create_date.day, &file->create_date.month, &file->create_date.year, &file->create_date.hour, &file->create_date.minute) < 0) {

		free(file);
		return NULL;
	}

	file->update_date = file->create_date;

	s.data[i] = '\0';

	while(i >= 0 && s.data[i] != '/')
		--i;

	if(i < 0 || sscanf(s.data + i + 1, "%ld", &file->size) < 0) {

		free(file);
		return NULL;
	}

	s.data[i] = '\0';

	while(i >= 0 && s.data[i] != '/')
		--i;

	if(i < 0) {

		free(file);
		return NULL;
	}

	strcpy(file->name, s.data + i + 1); 
	s.data[i] = '\0';		

	i = 0;

	while(s.data[i] && s.data[i] != '/')
		++i;

	if(!s.data[i]) {

		free(file);
		return NULL;
	}

	file->parent.data = NULL;
	cstring_set(&file->parent, s.data + i);
	
	return file;
}

Directory* create_directory_from_s(cstring s) {

	Directory* dir = malloc(sizeof(Directory));
	int i = s.size - 1;

	while(i >= 0 && s.data[i] != '/')
		--i;

	--i;

	while(i >= 0 && s.data[i] != '/')
		--i;

	if(i < 0 || sscanf(s.data + i + 1, "%hd-%hd-%hd/%hd:%hd", &dir->create_date.day, &dir->create_date.month, &dir->create_date.year, &dir->create_date.hour, &dir->create_date.minute) < 0) {

		free(dir);
		return NULL;
	}

	dir->update_date = dir->create_date;

	s.data[i] = '\0';

	while(i >= 0 && s.data[i] != '/')
		--i;

	if(i < 0 || sscanf(s.data + i + 1, "%d", &dir->count_elements) < 0) {

		free(dir);
		return NULL;
	}

	s.data[i] = '\0';

	while(i >= 0 && s.data[i] != '/')
		--i;

	if(i < 0) {

		free(dir);
		return NULL;
	}

	strcpy(dir->name, s.data + i + 1); 
	s.data[i] = '\0';		

	i = 0;

	while(s.data[i] && s.data[i] != '/')
		++i;

	dir->parent.data = NULL;

	if(s.data[i])
		cstring_set(&dir->parent, s.data + i);

	else
		cstring_set(&dir->parent, "");
	
	return dir;
}

NTree* create_ntree_from_file(const char* filename) {

	List treesDirectories = {NULL, NULL}, nodesFiles = {NULL, NULL};
	FILE* input = NULL;
	LinkedList *p, *q;
	NTree *root = NULL;
	cstring buffer = {0, 0};

	if(!(input = fopen(filename, "r"))) {

		fprintf(stderr, "Error %s : %s\n", filename, strerror(errno));
		exit(1);
	}

	while(readLine(input, &buffer) != EOF) {

		if(buffer.data[0] == 'D')
			add_list(&treesDirectories, create_node(create_directory_from_s(buffer), 'D'));
	
		else if(buffer.data[0] == 'B' || buffer.data[0] == 'T')
			add_list(&nodesFiles, create_node(create_file_from_s(buffer), 'F'));
	}

	for(p = treesDirectories.front; p; p = p->next) {

		NTree *parent = p->value, *child;

		for(q = treesDirectories.front; q != p; q = q->next) {

			child = q->value;

			if(dir_is_child(parent->value, child->value))
				insert_tree(parent, child);
		}

		q = q->next;

		for(; q; q = q->next) {

			child = q->value;

			if(dir_is_child(parent->value, child->value))
				insert_tree(parent, child);
		}

		for(q = nodesFiles.front; q; q = q->next) {

			child = q->value;

			if(file_is_child(parent->value, child->value))
				insert_tree(parent, child);
		}
	}


	for(p = treesDirectories.front; p; p = p->next) {

		NTree* node = p->value;

		if(!node->parent) {

			if(root) {

				fprintf(stderr, "Erro: Criação de várias pastas raiz\n");
				exit(1);
			}

			root = node;
		}
	}

	free(buffer.data);
	remove_nodes_list(&treesDirectories);
	remove_nodes_list(&nodesFiles);

	fclose(input);

	return root;
}

