#pragma once
#ifndef FS_LIB_LOADER_WAVEFRONT_OBJ_NEW
#define FS_LIB_LOADER_WAVEFRONT_OBJ_NEW

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../datatype/fs_list.h"
#include "../datatype/fs_rbtree.h"
#include "../datatype/fs_node.h"
#include "../datatype/fs_3d_data.h"
#include "../fs_stdfunc.h"
#include "../fs_opengl.h"
#include "../utils/fs_debug_print.h"
#include "../fs_node_render.h"

#include "2d_png.h"

#define CACHE_STRING_SIZE 128

typedef struct _FS_LoaderOBJ_DataContainer {
	float* vb_vertex;
	float* vb_normal;
	float* vb_texcoord;
	int* element_buffer;
	int vb_vertex_size, vb_normal_size, vb_texcoord_size;
	int element_buffer_size;

	float collision_radius;

	list* texture_list;
} FS_LoaderOBJ_DataContainer;

typedef struct _FS_VertexCacheData {
	char cache_string[CACHE_STRING_SIZE];
	int index;
} FS_VCData;

typedef struct _FS_ElementAttribIndexes {
	int pos;
	int uv;
	int normal;

	char cache_string[CACHE_STRING_SIZE];
	unsigned long cache_hash;
} FS_EAIndex;

typedef struct _FS_FaceElementIndexes {
	FS_EAIndex p[3];
} FS_FEIndex;

extern rbtree_node* cached_shader;
extern rbtree_node* cached_model;

node* FSnode_loadOBJ(node* target, const char* filename);

#endif