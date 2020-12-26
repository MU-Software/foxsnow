#ifndef FS_LIB_TYPE_NODE
#define FS_LIB_TYPE_NODE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct _node {
	struct _node* parent;
	struct _node* child;
	struct _node* child_last;
	struct _node* next;
	struct _node* prev;

	void (*in) (struct _node* target);
	void (*out)(struct _node* target);

	void* data;
	char* type;
} node;
typedef struct _node_head {
	node* node_start;
} node_head;

node* create_node(node* parent_to, void(*func_in), void(*func_out), void* data, char* type);

int free_node(node** target_node_ptr);

void node_print(node* target, int indent);

#endif