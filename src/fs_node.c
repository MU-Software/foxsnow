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

node_head render = { NULL };

node_data* create_node_data(char* name, int name_len, matrix* model_mat, matrix* view_mat) { //DONE
	node_data* result = (node_data*)calloc(1, sizeof(node_data));
	// Set name to NULL, because we don't have any name.
	result->name = name ? name : NULL;
	result->name_len = name ? name_len : 0;
	result->model_mat = model_mat ? model_mat :
						create_matrix(4, 4, 1, 0, 0, 0,
											0, 1, 0, 0,
											0, 0, 1, 0,
											0, 0, 0, 1);
	result->view_mat =  view_mat ? view_mat :
						create_matrix(4, 4, 1, 0, 0, 0,
											0, 1, 0, 0,
											0, 0, 1, 0,
											0, 0, 0, 1);
	// Set modelview matrix to NULL,
	// 1. mv mat will always be calculated when we access to modelview matrix,
	//    so there aren't any chances to access when mv mat is NULL.
	// 2. If we calloc mv mat, we need to free mv mat at calculation time.
	result->modelview_mat = NULL;
	return result;
}
node_data* copy_node_data(node_data* target_data) { //DONE
	node_data* result = (node_data*)calloc(1, sizeof(node_data));
	memcpy(result, target_data, sizeof(node_data));
	return result;
}
node_data* deepcopy_node_data(node_data* target_data) { //DONE
	node_data* result = (node_data*)calloc(1, sizeof(node_data));

	if (target_data->name != NULL && target_data->name_len){
		memcpy(result->name, target_data->name, sizeof(char)*target_data->name_len);
		result->name_len = target_data->name_len;
	}

	memcpy(result->model_mat, target_data->model_mat, sizeof(matrix));
	memcpy(result->view_mat, target_data->view_mat, sizeof(matrix));
	result->modelview_mat = NULL;
	return result;
}
int free_node_data(node_data** target_data_ptr) {
	free_matrix(&((*target_data_ptr)->model_mat));
	free_matrix(&((*target_data_ptr)->view_mat));
	free_matrix(&((*target_data_ptr)->modelview_mat));
	free((*target_data_ptr)->name);
	free((*target_data_ptr));
	target_data_ptr = NULL;
	return 0;
}

node* create_node(node* parent_to, void(*func_in), void(*func_out), node_data* data) {
	node* new_node = (node*)calloc(1, sizeof(node));
	new_node->in   = func_in;
	new_node->out  = func_out;

	if (!data) {
		data = (node_data*)calloc(1, sizeof(node_data));
		char data_noname[] = "NONAME";
		data->name = (char*)calloc(8, sizeof(char));
		strcpy(data->name, data_noname);

		data->model_mat = create_matrix(4, 4,
							1., 0., 0., 0.,
							0., 1., 0., 0.,
							0., 0., 1., 0.,
							0., 0., 0., 1.);
		data->view_mat  = create_matrix(4, 4,
							1., 0., 0., 0.,
							0., 1., 0., 0.,
							0., 0., 1., 0.,
							0., 0., 0., 1.);
		data->modelview_mat = create_matrix(4, 4,
								1., 0., 0., 0.,
								0., 1., 0., 0.,
								0., 0., 1., 0.,
								0., 0., 0., 1.);
	}
	new_node->data = data;

	if (parent_to != NULL) {
		if (parent_to->child != NULL) {
			node* target_neighbor = parent_to->child;
			while (target_neighbor->next != NULL)
				target_neighbor = target_neighbor->next;
			target_neighbor->next = new_node;
			new_node->prev = target_neighbor;
			new_node->parent = parent_to;
			parent_to->child_last = new_node;
		}
		else
			parent_to->child = new_node,
			parent_to->child_last = new_node,
			new_node->parent = parent_to;
	}
	return new_node;
}
node* copy_node(node* target) {
	node* result = (node*)calloc(1, sizeof(node));
	memcpy(result, target, sizeof(node));
	//TODO : COPY NODE DATA
	return target;
}

int free_node(node** target_node_ptr) {
	node* target_node = *target_node_ptr;
	// Return error when target_node == NULL
	if (target_node == NULL) return 1;
	// 자식 노드를 전부 날린다
	if (target_node->child != NULL)
		while (target_node->child)
			if (free_node(&(target_node->child))) return 1;
	// 주변에 같은 레벨의 노드가 있는지 확인하고, 참이면 내 레벨의 다음 노드를 전 노드에 이어준다
	if (target_node->next) {
		if (target_node->prev) {
			// 이러면 얘가 그냥 같은 레벨의 중간에 있다는거니깐 얘 앞뒤로 잘 이어주면 됨
			target_node->prev->next = target_node->next;
			target_node->next->prev = target_node->prev;
		}
		else {
			// 이 케이스는 얘가 같은 레벨의 첫번째였단거니깐 부모 child에 얘 다음 노드를 등록하고,
			// 얘 다음 노드의 prev는 NULL로 줘야 함
			target_node->parent->child = target_node->next;
			target_node->next->prev = NULL;
		}
	}
	else{
		if (target_node->prev) {
			// 만약 얘가 얘 트리에서 레벨의 마지막이었다면 부모의 child_last도 바꿔준다
			target_node->parent->child_last = target_node->prev;
			target_node->prev->next = NULL;
		}
		else {
			// 이건 그냥 얘가 외동이었단거니깐 부모의 child와 child_last를 날려주자
			target_node->parent->child = NULL;
			target_node->parent->child_last = NULL;
		}
	}

	// 마지막으로 데이터를 지우고, 이 노드 자체를 지우자
	free_node_data(&(target_node->data));
	free(target_node);
	*target_node_ptr = NULL;
	return 0;
}

void node_print(node* target, int indent) { //DONE
	if (!target) {
		printf("EMPTY NODE!");
		return;
	}
	int z = 0;
	for (z; z < indent; z++) printf(" ");
	printf("NODE %s\n", target->data->name);

	node* target_child = target->child;
	while (target_child != NULL) {
		node_print(target_child, indent+1);
		target_child = target_child->next;
	}
}
void reparent_node(node* target_node, node* parent_to) {
	if (target_node->parent != NULL) {
		// target_node.parent.child = 
	}
}
void node_postorder(node* target, int level) { //DONE
	if (!target) return;

	target->in(target);
	node* target_child = target->child;
	while(target_child) {
		node_postorder(target_child, level+1);
		target_child = target_child->next;
	}
	target->out(target);
}

void src_in(node* target) {
	matrix* tmp_mat_1 = mat_multiply(target->data->view_mat, target->data->model_mat);
	free_matrix(&(target->data->modelview_mat));
	target->data->modelview_mat = mat_multiply(target->parent->data->modelview_mat, tmp_mat_1);
	free_matrix(&tmp_mat_1);
}
void src_out(node* target) { //DONE
	printf("%s ", target->data->name);
}
char* create_dynamic_str(char* str, int size) { //DONE
	char* new_str = (char*)calloc(size+1, sizeof(char));
	strcpy(new_str, str);
	return new_str;
}

void node_test() {
	render.node_start = create_node(NULL, *src_in, *src_out, NULL);
	node* t_1  = render.node_start;
	node* t_2  = create_node(t_1, *src_in, *src_out, NULL);
	node* t_3  = create_node(t_1, *src_in, *src_out, NULL);
	node* t_4  = create_node(t_1, *src_in, *src_out, NULL);
	node* t_5  = create_node(t_2, *src_in, *src_out, NULL);
	node* t_6  = create_node(t_2, *src_in, *src_out, NULL);
	node* t_7  = create_node(t_2, *src_in, *src_out, NULL);
	node* t_8  = create_node(t_3, *src_in, *src_out, NULL);
	node* t_9  = create_node(t_3, *src_in, *src_out, NULL);
	node* t_10 = create_node(t_3, *src_in, *src_out, NULL);
	node* t_11 = create_node(t_4, *src_in, *src_out, NULL);
	node* t_12 = create_node(t_4, *src_in, *src_out, NULL);
	node* t_13 = create_node(t_4, *src_in, *src_out, NULL);
	node* t_14 = create_node(t_5, *src_in, *src_out, NULL);
	node* t_15 = create_node(t_5, *src_in, *src_out, NULL);
	node* t_16 = create_node(t_5, *src_in, *src_out, NULL);
	node* t_17 = create_node(t_6, *src_in, *src_out, NULL);
	node* t_18 = create_node(t_6, *src_in, *src_out, NULL);
	node* t_19 = create_node(t_6, *src_in, *src_out, NULL);
	node* t_20 = create_node(t_7, *src_in, *src_out, NULL);
	node* t_21 = create_node(t_7, *src_in, *src_out, NULL);
	node_print(t_1, 0);
	node_postorder(t_1, 0);

	free_node(&t_6);
	printf("Deleted Node 6!\n");
	printf("Node 6 %p\n", t_6);
	node_print(t_1, 0);

	free_node(&t_15);
	printf("Deleted Node 15!\n");
	printf("Node 15 %p\n", t_15);
	node_print(t_1, 0);

	if (!free_node(&t_15)) printf("Deleted Node 15!\n");
	else printf("ERROR while deleting Node 15\n");
	printf("Node 15 %p\n", t_15);
	node_print(t_1, 0);
}
// int main(void) {
// 	printf("HELLO WORLD!\n");
// 	node_test();
// }