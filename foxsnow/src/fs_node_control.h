#ifndef FS_LIB_NODE_CONTROL
#define FS_LIB_NODE_CONTROL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#endif

#include "fs_opengl.h"
#include "datatype/fs_matrix.h"
#include "datatype/fs_3d_data.h"
#include "datatype/fs_node.h"
#include "datatype/fs_rbtree.h"

extern rbtree_node* cached_shader;
extern rbtree_node* cached_model;

node* FSnode_setShader(node* target, const char* name);
node* FSnode_unsetShader(node* target);


#endif