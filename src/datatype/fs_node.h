#ifndef FS_NODE
	#define FS_NODE

	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>
	
	#include "fs_matrix.h"
	#include "fs_3d_data.h"

	typedef struct _node {
		struct _node* parent;
		struct _node* child;
		struct _node* child_last;
		struct _node* next;
		struct _node* prev;

		void (*in) (struct _node* target);
		void (*out)(struct _node* target);
		fs_3d_data* data;
	} node;
	typedef struct _node_head {
		node* node_start;
	} node_head;

	node* create_node(node* parent_to, void(*func_in), void(*func_out), fs_3d_data* data);
	node* copy_node(node* target);

	int free_node(node** target_node_ptr);

	void node_print(node* target, int indent);
	void reparent_node(node* target_node, node* parent_to);
	void node_postorder(node* target, int level);

	void first_src_in(node* target);
	void src_in(node* target);
	void src_out(node* target);

	void node_test();
#endif