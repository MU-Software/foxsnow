#ifndef FS_LIB_TYPE_3DDATA
#define FS_LIB_TYPE_3DDATA

#include <math.h>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#ifndef __glew_h__
#include <GL/glew.h>
#endif

#include "fs_matrix.h"
#include "fs_list.h"
#include "fs_rbtree.h"
#include "../fs_opengl.h"

typedef struct _FS_Type_3D_PolyModel {
    char* name;

    int model_vertex_size;
    float* model_vertex_array;
    int* model_index_array;

    GLuint vertex_array;
    GLuint element_buffer;
    unsigned int element_buffer_size;

    GLuint vertex_buffer_position;
    GLuint vertex_buffer_normal;
    list* vertex_buffer_texcoord_list;
    list* texture_list;

    float collision_radius;

    unsigned int reference_count;
} FS_Type_3D_PolyModel;

typedef struct _fs_3d_data {
    bool do_backface_cull;

    rbtree_node* model;
    rbtree_node* shader;

    float collision_radius;

    float pos[3];
    float scale[3];
    float rotate[3];

    matrix* _transform;
    matrix* _scaling;
    matrix* _rotating_x;
    matrix* _rotating_y;
    matrix* _rotating_z;
    matrix* _rotating;

    matrix* model_mat;
    matrix* model_mat_without_scale;
    matrix* cumulative_model_mat;
    matrix* cumulative_model_mat_without_scale;
} fs_3d_data;

fs_3d_data* create_data();
int free_data(fs_3d_data** target_container_ptr);

fs_3d_data* apply_data(fs_3d_data* target);

fs_3d_data* set_pos(fs_3d_data* target, float x, float y, float z);
fs_3d_data* set_x(fs_3d_data* target, float x);
fs_3d_data* set_y(fs_3d_data* target, float y);
fs_3d_data* set_z(fs_3d_data* target, float z);

fs_3d_data* set_scale(fs_3d_data* target, float sx, float sy, float sz);
fs_3d_data* set_sx(fs_3d_data* target, float sx);
fs_3d_data* set_sy(fs_3d_data* target, float sy);
fs_3d_data* set_sz(fs_3d_data* target, float sz);

fs_3d_data* set_rotate(fs_3d_data* target, float h, float p, float r);
fs_3d_data* set_h(fs_3d_data* target, float h);
fs_3d_data* set_p(fs_3d_data* target, float p);
fs_3d_data* set_r(fs_3d_data* target, float r);

float* get_pos(fs_3d_data target);
float* get_scale(fs_3d_data target);
float* get_rotate(fs_3d_data target);

#endif