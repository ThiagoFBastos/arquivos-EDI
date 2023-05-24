#ifndef NTREE_H
#define NTREE_H

typedef struct NTree NTree;

struct NTree {

	void* value;
	char type;

	NTree *child, *sibling, *parent;
};

NTree* create_node(void*, char);

int is_direct_child(NTree*, const char*);

int add_node(NTree*, void*, char);
int remove_node(NTree*, NTree*);

int insert_tree(NTree*, NTree*);

void dispose(NTree*);

#endif
