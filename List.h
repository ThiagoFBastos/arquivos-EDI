#ifndef LIST_H
#define LIST_H

#include "NTree.h"

typedef struct LinkedList LinkedList;

struct LinkedList {

	NTree *value;
	LinkedList *next;
};

typedef struct List {

	LinkedList *front, *back;

} List;

void add_list(List*, NTree*);
void remove_nodes_list(List*);

#endif
