#pragma once
#ifndef FS_LIB_TYPE_DOUBLELIST
#define FS_LIB_TYPE_DOUBLELIST

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "../utils/fs_debug_print.h"

typedef struct _list_element {
	struct _list_element* prev;
	struct _list_element* next;
	void* data;
	char* type;
} list_element;

typedef struct _list {
	list_element* head;
	list_element* tail;

	unsigned int length;
} list;

list* create_list();
int free_list(list** target_list_ptr);

list_element* list_get_element(list* target_list, int index);
char* list_get(list* target_list, int index);

void list_insert(list* target_list, int index, void* data, char* type);
void list_append(list* target_list, void* data, char* type);

void list_remove(list* target_list, int index, bool free_data);
int list_clear(list* target_list);
char* list_pop(list* target_list, int index);

#endif