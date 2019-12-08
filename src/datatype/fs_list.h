#ifndef FS_LIST
    #define FS_LIST

    #include <stdio.h>
    #include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>

	#include "fs_3d_data.h"
    
	typedef struct _list {
		struct _list* next;
		struct _list* prev;

		struct _list* head;
		struct _list* tail;

		int index;
		int length;
		
        fs_3d_data* data;
	} list;
	typedef struct _list_head {
		list* list_start;
        int length;
	} list_head;

	list* create_list_element(list* parent_to, fs_3d_data* data);
	int free_list_element(list** target_list_ptr);
	int free_list(list** target_list_ptr);

	void list_print(list* target, int indent);

	void list_test();

#endif