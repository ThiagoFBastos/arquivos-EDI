#include "List.h"

#include <stdlib.h>

void add_list(List *L, NTree *value) {

	LinkedList *no = malloc(sizeof(LinkedList));

	no->next = NULL;
	no->value = value;

	if(L->back)
		L->back->next = no;
	else
		L->front = no;

	L->back = no;
}

void remove_nodes_list(List *L) {

	LinkedList *no, *next;

	for(no = L->front; no; no = next) {

		next = no->next;
		free(no);
	}
}
