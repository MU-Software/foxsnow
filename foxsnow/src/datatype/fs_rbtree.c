#include "fs_rbtree.h"

void _rbtree_black_insert(rbtree_node** root, rbtree_node* new) {
    rbtree_node* y;
    rbtree_node* x;

    y = NULL;
    x = *root;
    while (x) {
        y = x;
        if (new->key < x->key)
            x = x->left;
        else
            x = x->right;
    }
    new->parent = y;
    if (new->key > y->key)
        y->right = new;
    else
        y->left = new;
    new->color = RED;
    _rbtree_insert_fix(root, new);
}
void rbtree_insert(rbtree_node** root, int key, void* data, char* type) {
    rbtree_node* z = (rbtree_node*)calloc(1, sizeof(rbtree_node));
    ALLOC_FAILCHECK(z);
    z->key = key;
    z->data = data;
    z->type = type;

    z->left = NULL;
    z->right = NULL;
    z->parent = NULL;
    if (*root == NULL) {
        z->color = BLACK;
        (*root) = z;
    }
    else _rbtree_black_insert(root, z);
}

int is_uncle_red(rbtree_node* z) {
    rbtree_node* y;

    y = z->parent->parent;
    y = z->parent == y->left ? y->right : y->left;
    if (y && y->color == RED) {
        y->color = BLACK;
        z->parent->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent;
        return 1;
    }
    else
        return 0;
}
void _rbtree_case_left(rbtree_node** root, rbtree_node* z) {
    enum rbtree_color tmp_color;

    if (z == z->parent->left) {
        tmp_color = z->parent->color;
        z->parent->color = z->parent->parent->color;
        z->parent->parent->color = tmp_color;
        _rbtree_rotate_right(root, z->parent->parent);
    }
    else {
        tmp_color = z->color;
        z->color = z->parent->parent->color;
        z->parent->parent->color = tmp_color;
        _rbtree_rotate_left(root, z->parent);
        _rbtree_rotate_right(root, z->parent->parent);
    }
}
void _rbtree_case_right(rbtree_node** root, rbtree_node* z) {
    enum rbtree_color tmp_color;

    if (z == z->parent->right) {
        tmp_color = z->parent->color;
        z->parent->color = z->parent->parent->color;
        z->parent->parent->color = tmp_color;
        _rbtree_rotate_left(root, z->parent->parent);
    }
    else {
        tmp_color = z->color;
        z->color = z->parent->parent->color;
        z->parent->parent->color = tmp_color;
        _rbtree_rotate_right(root, z->parent);
        _rbtree_rotate_left(root, z->parent->parent);
    }
}
void _rbtree_insert_fix(rbtree_node** root, rbtree_node* z) {
    while (z != *root && z->parent != *root && z->parent->color == RED) {
        if (is_uncle_red(z))
            continue;
        else if (z->parent == z->parent->parent->left)
            _rbtree_case_left(root, z);
        else if (z->parent == z->parent->parent->right)
            _rbtree_case_right(root, z);
    }
    (*root)->color = BLACK;
}
void _rbtree_rotate_left(rbtree_node** root, rbtree_node* x) {
    rbtree_node* y;

    if (!x || !x->right)
        return;
    y = x->right;
    x->right = y->left;
    if (x->right != NULL)
        x->right->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL)
        (*root) = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}
void _rbtree_rotate_right(rbtree_node** root, rbtree_node* y) {
    rbtree_node* x;

    if (!y || !y->left)
        return;
    x = y->left;
    y->left = x->right;
    if (x->right != NULL)
        x->right->parent = y;
    x->parent = y->parent;
    if (x->parent == NULL)
        (*root) = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else y->parent->right = x;
    x->right = y;
    y->parent = x;
}

void _rbtree_free_recursive(rbtree_node* start) {
    rbtree_node* tmp;

    while (start->left || start->right)
        start = start->left ? start->left : start->right;
    tmp = start->parent;
    free(start);
    if (tmp) {
        if (start == tmp->left)
            tmp->left = NULL;
        else
            tmp->right = NULL;
        _rbtree_free_recursive(tmp);
    }
}
void rbtree_free(rbtree_node** root) {
    _rbtree_free_recursive(*root);
    *root = NULL;
}

rbtree_node* rbtree_search(rbtree_node* root, unsigned long key) {
    if (root == NULL) return NULL;

    if (key == root->key) return root;
    else if (key < root->key) return rbtree_search(root->left, key);
    else if (key > root->key) return rbtree_search(root->right, key);
    return NULL;
}

void rbtree_print(rbtree_node* root) {
    if (root == NULL) return;
    rbtree_print(root->left);
    printf("%d\n", root->key);
    rbtree_print(root->right);
}
