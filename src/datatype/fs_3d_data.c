#include "fs_3d_data.h"

char* create_dynamic_str(char* str, int size) { //DONE
	char* new_str = (char*)calloc(size+1, sizeof(char));
	strcpy(new_str, str);
	return new_str;
}

fs_3d_data* swallow_copy_data(fs_3d_data* target_data) { //DONE
	fs_3d_data* result = (fs_3d_data*)calloc(1, sizeof(fs_3d_data));
	memcpy(result, target_data, sizeof(fs_3d_data));
	return result;
}
fs_3d_data* deep_copy_data(fs_3d_data* target_data) { //DONE
	fs_3d_data* result = (fs_3d_data*)calloc(1, sizeof(fs_3d_data));
	memcpy(result, target_data, sizeof(fs_3d_data));
    
    if (target_data->_transform) {
        result->_transform = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
                                                 0.0f, 1.0f, 0.0f, 0.0f,
                                                 0.0f, 0.0f, 1.0f, 0.0f,
                                                 0.0f, 0.0f, 0.0f, 1.0f);
        memcpy(result->_transform->mat, target_data->_transform->mat, sizeof(float)*4*4);
    }
    if (target_data->_scaling) {
        result->_scaling = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
                                               0.0f, 1.0f, 0.0f, 0.0f,
                                               0.0f, 0.0f, 1.0f, 0.0f,
                                               0.0f, 0.0f, 0.0f, 1.0f);
        memcpy(result->_scaling->mat, target_data->_scaling->mat, sizeof(float)*4*4);
    }
    if (target_data->_rotating_x) {
        result->_rotating_x = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
                                                  0.0f, 1.0f, 0.0f, 0.0f,
                                                  0.0f, 0.0f, 1.0f, 0.0f,
                                                  0.0f, 0.0f, 0.0f, 1.0f);
        memcpy(result->_rotating_x->mat, target_data->_rotating_x->mat, sizeof(float)*4*4);
    }
    if (target_data->_rotating_y) {
        result->_rotating_y = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
                                                  0.0f, 1.0f, 0.0f, 0.0f,
                                                  0.0f, 0.0f, 1.0f, 0.0f,
                                                  0.0f, 0.0f, 0.0f, 1.0f);
        memcpy(result->_rotating_y->mat, target_data->_rotating_y->mat, sizeof(float)*4*4);
    }
    if (target_data->_rotating_z) {
        result->_rotating_z = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
                                                  0.0f, 1.0f, 0.0f, 0.0f,
                                                  0.0f, 0.0f, 1.0f, 0.0f,
                                                  0.0f, 0.0f, 0.0f, 1.0f);
        memcpy(result->_rotating_z->mat, target_data->_rotating_z->mat, sizeof(float)*4*4);
    }
    if (target_data->_rotating) {
        result->_rotating  = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
                                                 0.0f, 1.0f, 0.0f, 0.0f,
                                                 0.0f, 0.0f, 1.0f, 0.0f,
                                                 0.0f, 0.0f, 0.0f, 1.0f);
        memcpy(result->_rotating->mat, target_data->_rotating->mat, sizeof(float)*4*4);
    }
    if (target_data->model_mat) {
        result->model_mat  = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
                                                 0.0f, 1.0f, 0.0f, 0.0f,
                                                 0.0f, 0.0f, 1.0f, 0.0f,
                                                 0.0f, 0.0f, 0.0f, 1.0f);
        memcpy(result->model_mat->mat, target_data->model_mat->mat, sizeof(float)*4*4);
    }
    if (target_data->cumulative_model_mat) {
        result->cumulative_model_mat  = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
                                                            0.0f, 1.0f, 0.0f, 0.0f,
                                                            0.0f, 0.0f, 1.0f, 0.0f,
                                                            0.0f, 0.0f, 0.0f, 1.0f);
        memcpy(result->cumulative_model_mat->mat, target_data->cumulative_model_mat->mat, sizeof(float)*4*4);
    }

	return result;
}

fs_3d_data* create_data(char* name, int name_len) { //DONE
	fs_3d_data* result = (fs_3d_data*)calloc(1, sizeof(fs_3d_data));

	result->name = name ? name : NULL;
	result->name_len = name ? name_len : 0;

    result->model_vertex_array = NULL;
    result->model_index_array = NULL;

    int z;
    for(z=0; z<3; z++) {
        result->pos[z] = 0.0f;
        result->scale[z] = 1.0f;
        result->rotate[z] = 0.0f;
    }

    result->_transform  = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
											  0.0f, 1.0f, 0.0f, 0.0f,
											  0.0f, 0.0f, 1.0f, 0.0f,
											  0.0f, 0.0f, 0.0f, 1.0f);
    result->_scaling    = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
											  0.0f, 1.0f, 0.0f, 0.0f,
											  0.0f, 0.0f, 1.0f, 0.0f,
											  0.0f, 0.0f, 0.0f, 1.0f);

    result->_rotating_x = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
											  0.0f, 1.0f, 0.0f, 0.0f,
											  0.0f, 0.0f, 1.0f, 0.0f,
											  0.0f, 0.0f, 0.0f, 1.0f);
    result->_rotating_y = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
											  0.0f, 1.0f, 0.0f, 0.0f,
											  0.0f, 0.0f, 1.0f, 0.0f,
											  0.0f, 0.0f, 0.0f, 1.0f);
    result->_rotating_z = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
											  0.0f, 1.0f, 0.0f, 0.0f,
											  0.0f, 0.0f, 1.0f, 0.0f,
											  0.0f, 0.0f, 0.0f, 1.0f);
    result->_rotating   = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
											  0.0f, 1.0f, 0.0f, 0.0f,
											  0.0f, 0.0f, 1.0f, 0.0f,
											  0.0f, 0.0f, 0.0f, 1.0f);

	result->model_mat   = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
											  0.0f, 1.0f, 0.0f, 0.0f,
											  0.0f, 0.0f, 1.0f, 0.0f,
											  0.0f, 0.0f, 0.0f, 1.0f);
	result->cumulative_model_mat   = create_matrix(4, 4, 1.0f, 0.0f, 0.0f, 0.0f,
										                 0.0f, 1.0f, 0.0f, 0.0f,
										                 0.0f, 0.0f, 1.0f, 0.0f,
										                 0.0f, 0.0f, 0.0f, 1.0f);
	return result;
}
int free_data(fs_3d_data** target_data_ptr) { //DONE
    free((*target_data_ptr)->name);

    glUseProgram(0);
    glDisableVertexAttribArray((*target_data_ptr)->vertex_array);
    glDetachShader((*target_data_ptr)->program, (*target_data_ptr)->vert_shader);
    glDetachShader((*target_data_ptr)->program, (*target_data_ptr)->frag_shader);
    glDeleteProgram((*target_data_ptr)->program);
    glDeleteShader((*target_data_ptr)->vert_shader);
    glDeleteShader((*target_data_ptr)->frag_shader);
    glDeleteBuffers(1, &((*target_data_ptr)->vertex_buffer));
    glDeleteBuffers(1, &((*target_data_ptr)->element_buffer));
    glDeleteVertexArrays(1, &((*target_data_ptr)->vertex_array));
    // glDeleteTextures(1, &m_tex);

    if ((*target_data_ptr)->model_vertex_array)
        free((*target_data_ptr)->model_vertex_array);
    if ((*target_data_ptr)->model_index_array)
        free((*target_data_ptr)->model_index_array);

    free_matrix(&((*target_data_ptr)->_transform));
    free_matrix(&((*target_data_ptr)->_scaling));
    
    if ((*target_data_ptr)->_rotating_x)
        free_matrix(&((*target_data_ptr)->_rotating_x));
    if ((*target_data_ptr)->_rotating_y)
        free_matrix(&((*target_data_ptr)->_rotating_y));
    if ((*target_data_ptr)->_rotating_z)
        free_matrix(&((*target_data_ptr)->_rotating_z));
    if ((*target_data_ptr)->_rotating)
        free_matrix(&((*target_data_ptr)->_rotating));
    
    if ((*target_data_ptr)->model_mat)
        free_matrix(&((*target_data_ptr)->model_mat));
    if ((*target_data_ptr)->cumulative_model_mat)
        free_matrix(&((*target_data_ptr)->cumulative_model_mat));

    free((*target_data_ptr));
	target_data_ptr = NULL;

    return 0;
}

fs_3d_data* apply_data(fs_3d_data* target) { //DONE
    target->_transform->mat[12] = target->pos[0];
    target->_transform->mat[13] = target->pos[1];
    target->_transform->mat[14] = target->pos[2];

    target->_scaling->mat[0]  = target->scale[0];
    target->_scaling->mat[5]  = target->scale[1];
    target->_scaling->mat[10] = target->scale[2];

    target->_rotating_x->mat[ 5] =  cos(target->rotate[0]);
    target->_rotating_x->mat[ 6] = -sin(target->rotate[0]);
    target->_rotating_x->mat[ 9] =  sin(target->rotate[0]);
    target->_rotating_x->mat[10] =  cos(target->rotate[0]);

    target->_rotating_y->mat[ 0] =  cos(target->rotate[1]);
    target->_rotating_y->mat[ 2] =  sin(target->rotate[1]);
    target->_rotating_y->mat[ 8] = -sin(target->rotate[1]);
    target->_rotating_y->mat[10] =  cos(target->rotate[1]);

    target->_rotating_z->mat[ 0] =  cos(target->rotate[2]);
    target->_rotating_z->mat[ 1] = -sin(target->rotate[2]);
    target->_rotating_z->mat[ 4] =  sin(target->rotate[2]);
    target->_rotating_z->mat[ 5] =  cos(target->rotate[2]);

    matrix* tmp_multiply_mat = NULL;

    if (!(target->_rotating)) free_matrix(target->_rotating);
    tmp_multiply_mat = mat_square_multiply(target->_rotating_x, target->_rotating_y);
    target->_rotating = mat_square_multiply(target->_rotating_z, tmp_multiply_mat);
    free_matrix(tmp_multiply_mat);

    if (!(target->model_mat)) free_matrix(target->model_mat);
    tmp_multiply_mat = mat_square_multiply(target->_scaling, target->_rotating);
    target->model_mat = mat_square_multiply(tmp_multiply_mat, target->_transform);
    free_matrix(tmp_multiply_mat);
    return target;
}

fs_3d_data* set_pos(fs_3d_data* target, float x, float y, float z) { //DONE
    target->pos[0] = x;
    target->pos[1] = y;
    target->pos[2] = z;
    return apply_data(target);
}
fs_3d_data* set_x(fs_3d_data* target, float x) { //DONE
    target->pos[0] = x;
    return apply_data(target);
}
fs_3d_data* set_y(fs_3d_data* target, float y) { //DONE
    target->pos[1] = y;
    return apply_data(target);
}
fs_3d_data* set_z(fs_3d_data* target, float z) { //DONE
    target->pos[2] = z;
    return apply_data(target);
}

fs_3d_data* set_scale(fs_3d_data* target, float sx, float sy, float sz) { //DONE
    target->scale[0] = sx;
    target->scale[1] = sy;
    target->scale[2] = sz;
    return apply_data(target);
}
fs_3d_data* set_sx(fs_3d_data* target, float sx) { //DONE
    target->scale[0] = sx;
    return apply_data(target);
}
fs_3d_data* set_sy(fs_3d_data* target, float sy) { //DONE
    target->scale[1] = sy;
    return apply_data(target);
}
fs_3d_data* set_sz(fs_3d_data* target, float sz) { //DONE
    target->scale[2] = sz;
    return apply_data(target);
}

fs_3d_data* set_rotate(fs_3d_data* target, float h, float p, float r) { //DONE
    target->rotate[0] = h;
    target->rotate[1] = p;
    target->rotate[2] = r;
    return apply_data(target);
}
fs_3d_data* set_h(fs_3d_data* target, float h) { //DONE
    target->rotate[0] = h;
    return apply_data(target);
}
fs_3d_data* set_p(fs_3d_data* target, float p) { //DONE
    target->rotate[1] = p;
    return apply_data(target);
}
fs_3d_data* set_r(fs_3d_data* target, float r) { //DONE
    target->rotate[2] = r;
    return apply_data(target);
}


float* get_pos(fs_3d_data target) { //DONE
    float* result = (float*)malloc(sizeof(float)*3);
    int z;
    for(z = 0; z < 3; z++)
        result[z] = target.pos[z];
    return result;
}
float* get_scale(fs_3d_data target) { //DONE
    float* result = (float*)malloc(sizeof(float)*3);
    int z;
    for(z = 0; z < 3; z++)
        result[z] = target.scale[z];
    return result;
}
float* get_rotate(fs_3d_data target) { //DONE
    float* result = (float*)malloc(sizeof(float)*3);
    int z;
    for(z = 0; z < 3; z++)
        result[z] = target.rotate[z];
    return result;
}