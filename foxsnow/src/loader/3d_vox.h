#ifndef FS_LIB_LOADER_MAGICAVOXEL_VOX
#define FS_LIB_LOADER_MAGICAVOXEL_VOX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "../datatype/fs_list.h"
#include "../datatype/fs_rbtree.h"
#include "../datatype/fs_node.h"
#include "../datatype/fs_3d_data.h"
#include "../fs_stdfunc.h"
#include "../fs_opengl.h"
#include "../utils/fs_debug_print.h"
#include "../fs_node_render.h"

#include "2d_png.h"

extern unsigned int FS_LoaderVox_DefaultPalette[256];

typedef struct _FS_LoaderVox_Voxel {
	int x;
	int y;
	int z;
	
	byte color_index;
} FS_LoaderVox_Voxel;

typedef struct _FS_LoaderVox_Model {
	int size_x;
	int size_y;
	int size_z;

	list* voxels;
} FS_LoaderVox_Model;

typedef struct _FS_LoaderVox_Palette {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} FS_LoaderVox_Palette;

typedef struct _FS_LoaderVox_MaterialV1 {
    int id; // [1-255]

	// material type
	//  - 0 : diffuse
	//  - 1 : metal
	//  - 2 : glass
	//  - 3 : emissive
	int material_type;

	// material weight
	//  - diffuse  : 1.0
	//  - metal    : (0.0 - 1.0] : blend between metal and diffuse material
	//  - glass    : (0.0 - 1.0] : blend between glass and diffuse material
	//  - emissive : (0.0 - 1.0] : self - illuminated material
	float material_weight;

	bool plastic;
	bool roughness;
	bool specular;
	bool ior;
	bool attenuation;
	bool power;
	bool glow;
	bool is_total_power;

	float normalized_property;

} FS_LoaderVox_MaterialV1;

int loadVOX(char* filename);
node* FSnode_loadVOX(node* target, const char* filename);

#endif