#pragma once
#ifndef FS_LIB_TYPE_RBTREE
#define FS_LIB_TYPE_RBTREE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

#include "../utils/fs_debug_print.h"

enum rbtree_color {
    BLACK = 0,
    RED
};

typedef struct _rbtree_node {
    uint32_t key;
    void* data;
    char* type;

    enum rbtree_color color;
    struct _rbtree_node* left;
    struct _rbtree_node* right;
    struct _rbtree_node* parent;
} rbtree_node;

void _rbtree_black_insert(rbtree_node** root, rbtree_node* new);
void rbtree_insert(rbtree_node** root, int key, void* data, char* type);

int is_uncle_red(rbtree_node* z);
void _rbtree_case_left(rbtree_node** root, rbtree_node* z);
void _rbtree_case_right(rbtree_node** root, rbtree_node* z);
void _rbtree_insert_fix(rbtree_node** root, rbtree_node* z);
void _rbtree_rotate_left(rbtree_node** root, rbtree_node* x);
void _rbtree_rotate_right(rbtree_node** root, rbtree_node* y);

void _rbtree_free_recursive(rbtree_node* start);
void rbtree_free(rbtree_node** root);

void rbtree_print(rbtree_node* root);
rbtree_node* rbtree_search(rbtree_node* root, unsigned long key);
#endif