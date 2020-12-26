#include "fs_list.h"

list* create_list() {
	list* result = (list*)calloc(1, sizeof(list));
	ALLOC_FAILCHECK(result);

	result->head = NULL;
	result->tail = NULL;
	result->length = 0;

	return result;
}
int free_list(list** target_list_ptr) {
	list* target_list = *target_list_ptr;
	if (target_list == NULL) return 1;

	list_clear(target_list);

	free(target_list);
	*target_list_ptr = NULL;
	return 0;
}

list_element* list_get_element(list* target_list, int index) {
	assert(target_list != NULL); // check SEGFAULT on nullable arg target_list
	assert(target_list->head != NULL); // check list head isn't NULL
	assert(target_list->tail != NULL); // check list tail isn't NULL
	assert(target_list->length != 0); // check list isn't blank
	if (index >= 0)
		assert(index < target_list->length); // check Index out of range

	int target_index = index;
	// target_index can be negative,
	// in this case, -1 will select last element,
	// and -2 will select before last element.
	// but target_index + target_list->length must not be negative.
	if (target_index < 0) {
		target_index += target_list->length;
		assert(target_index >= 0); // inverse index positive check
	}

	if (target_index == 0) {
		return target_list->head;
	}
	else if (target_index == ((int)target_list->length - 1)) {
		return target_list->tail;
	}
	else {
		list_element* target_element = target_list->head;
		int z;
		for (z = 0; (z < target_index) && target_element; z++) {
			target_element = target_element->next;
		}

		assert(target_element != NULL); // check NULL on <for> result.
		return target_element;
	}
}
char* list_get(list* target_list, int index) {
	return list_get_element(target_list, index)->data;
}

void list_insert(list* target_list, int index, void* data, char* type) {
	assert(target_list != NULL); // check target_list is NULL
	if (target_list->length == 0) {
		// When list is empty
		list_element* new_element = (list_element*)calloc(1, sizeof(list_element));
		ALLOC_FAILCHECK(new_element);
		new_element->data = data;
		new_element->type = type;

		target_list->head = new_element;
		target_list->tail = new_element;
		target_list->length = 1;

		return;
	}

	int target_index = index;
	// target_index can be negative,
	// in this case, -1 will make this function to add last,
	// and -2 will add element before last element.
	// but target_index + (target_list->length+1) must not be negative.
	if (target_index < 0) {
		target_index += target_list->length + 1;
		assert(target_index >= 0); // inverse index positive check
	}
	// target_index can bigger than target_list->length,
	// in this case, insert position will be the last.
	else if (target_index >= (int)target_list->length) {
		target_index = target_list->length;
	}

	list_element* new_element = (list_element*)calloc(1, sizeof(list_element));
	ALLOC_FAILCHECK(new_element);
	new_element->data = data;
	new_element->type = type;

	if (target_index == 0) {
		// if insert position is first
		target_list->head->prev = new_element;
		new_element->next = target_list->head;
		target_list->head = new_element;
	}
	else if (target_index == target_list->length) {
		// if insert position is last
		target_list->tail->next = new_element;
		new_element->prev = target_list->tail;
		target_list->tail = new_element;
	}
	else {
		list_element* prev_element = list_get_element(target_list, target_index - 1);
		assert(prev_element != NULL); // element null check

		new_element->prev = prev_element;
		new_element->next = prev_element->next;
		prev_element->next->prev = new_element;
		prev_element->next = new_element;
	}
	target_list->length++;
}
void list_append(list* target_list, void* data, char* type) {
	list_insert(target_list, -1, data, type);
}

void list_remove(list* target_list, int index, bool free_data) {
	list_element* target_element = list_get_element(target_list, index);
	assert(target_element != NULL); // check NULL on <list_get_element> result

	int target_index = index;
	// target_index can be negative,
	// in this case, -1 will select last element,
	// and -2 will select before last element.
	// but target_index + target_list->length must not be negative.
	if (target_index < 0) {
		target_index += target_list->length;
		assert(target_index >= 0); // inverse index positive check
	}

	if (target_list->length == 1) {
		target_list->head = NULL;
		target_list->tail = NULL;
	}
	else if (target_index == 0) {
		assert(target_list->head->next != NULL);
		target_list->head = target_list->head->next;
		target_list->head->prev = NULL;
	}
	else if (target_index == ((int)target_list->length - 1)) {
		target_list->tail = target_list->tail->prev;
		target_list->tail->next = NULL;
	}
	else {
		target_element->prev->next = target_element->next;
		target_element->next->prev = target_element->prev;
	}

	if (free_data) {
		// free is free from null pointer exception
		free(target_element->data);
		free(target_element->type);
	}

	free(target_element);
	target_list->length--;
}
int list_clear(list* target_list) {
	int removed_elements = (int)target_list->length;

	int z;
	for (z = 0; z < removed_elements; z++) {
		list_remove(target_list, 0, true);
	}

	return removed_elements;
}
char* list_pop(list* target_list, int index) {
	char* result = list_get(target_list, index);

	list_element* target_element = list_get_element(target_list, index);
	assert(target_element != NULL); // check NULL on <list_get_element> result
	target_element->data = NULL;

	list_remove(target_list, index, false);
	return result;
}

int list_index(list* target_list, char* data) {
	assert(data != NULL); // check data NULL

	int current_index;
	for (current_index = 0; current_index < target_list->length; current_index++) {
		if (list_get(target_list, current_index) == data)
			return current_index;
	}
	return -1;
}

void list_print(list* target_list) {
	list_element* current_element = target_list->head;
	int z = 0;
	while (current_element) {
		printf("INDEX:%d TYPE:%s\n",
			z,
			current_element->type);

		z++;
		current_element = current_element->next;
	}
}
