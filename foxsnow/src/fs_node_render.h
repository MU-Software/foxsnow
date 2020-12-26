#ifndef FS_LIB_NODE_RENDER
#define FS_LIB_NODE_RENDER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs_stdfunc.h"
#include "utils/fs_debug_print.h"
#include "fs_opengl.h"
#include "datatype/fs_matrix.h"
#include "datatype/fs_3d_data.h"
#include "datatype/fs_node.h"

extern unsigned long long frame_number;
extern int current_resolution_x;
extern int current_resolution_y;

typedef struct _fs_camera {
	float pos[3];
	float lookPoint[3];
	node* parent;
	matrix* cam_matrix;
} FS_Camera;

node_head render;
node_head render2d;

FS_Camera camera;

matrix* FS_ViewMatrix;
matrix* FS_ProjectionMatrix;

void commitCamera();

void renderNodeIn(node* target);
void renderNodeOut(node* target);
int renderNode(node* target, int level);

#endif