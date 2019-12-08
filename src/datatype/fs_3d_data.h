#ifndef FS_3D_DATA
    #define FS_3D_DATA

    #include <math.h>

    #ifndef GLEW_STATIC
        #define GLEW_STATIC
    #endif
    #ifndef __glew_h__
        #include <GL/glew.h>
    #endif

    #include "fs_matrix.h"

    typedef struct _fs_3d_data {
		char* name;
		int name_len;

		GLuint vert_shader;
		GLuint frag_shader;
		GLuint program;

		GLuint vertex_array;
		GLuint vertex_buffer;
		GLuint element_buffer;

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
        matrix* cumulative_model_mat;
	} fs_3d_data;

    fs_3d_data* swallow_copy_data(fs_3d_data* target_data);
    fs_3d_data* deep_copy_data(fs_3d_data* target_data);

    fs_3d_data* create_data(char* name, int name_len);
    int free_data(fs_3d_data** target_data_ptr);

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