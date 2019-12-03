#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "fs_matrix.h"

typedef struct _node_data {
	char* name;
	int name_len;
	matrix* model_mat;
	matrix* view_mat;
	matrix* modelview_mat;
} node_data;
typedef struct _node {
	struct _node* parent;
	struct _node* child;
	struct _node* child_last;
	struct _node* next;
	struct _node* prev;

	void (*in) (struct _node* target);
	void (*out)(struct _node* target);
	node_data* data;
} node;
typedef struct _node_head {
	node* node_start;
} node_head;

node_head render;

char* create_dynamic_str(char* str, int size);

node_data* create_node_data(char* name, int name_len, matrix* model_mat, matrix* view_mat);
node_data* copy_node_data(node_data* target_data);
node_data* deepcopy_node_data(node_data* target_data);
int free_node_data(node_data** target_data_ptr);

node* create_node(node* parent_to, void(*func_in), void(*func_out), node_data* data);
node* copy_node(node* target);

int free_node(node** target_node_ptr);

void node_print(node* target, int indent);
void reparent_node(node* target_node, node* parent_to);
void node_postorder(node* target, int level);

void first_src_in(node* target);
void src_in(node* target);
void src_out(node* target);

void node_test();