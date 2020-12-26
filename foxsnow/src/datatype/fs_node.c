#include "fs_node.h"

node* create_node(node* parent_to, void(*func_in), void(*func_out), void* data, char* type) {
	node* new_node = (node*)calloc(1, sizeof(node));
	if (new_node == NULL) {
		printf("CALLOC FAILED on create_node!\n");
		exit(1);
	}

	new_node->in = func_in;
	new_node->out = func_out;

	new_node->data = data;
	new_node->type = type;

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

int free_node(node** target_node_ptr) { //DONE
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
	else {
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
	free(target_node->data);
	free(target_node);
	*target_node_ptr = NULL;
	return 0;
}

void node_print(node* target, int indent) {
	if (!target) {
		printf("EMPTY NODE!");
		return;
	}
	int z = 0;
	for (z; z < indent; z++) printf(" ");
	printf("NODE %s\n", target->type);

	node* target_child = target->child;
	while (target_child != NULL) {
		node_print(target_child, indent + 1);
		target_child = target_child->next;
	}
}
