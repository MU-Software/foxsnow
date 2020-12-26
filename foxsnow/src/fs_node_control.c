#include "fs_node_control.h"

node* FSnode_setShader(node* target, const char* name) {
    NULL_CHECK(target);
    NULL_CHECK(name);
    fs_3d_data* target_data = (fs_3d_data*)target->data;
    NULL_CHECK(target_data);

    target_data->shader_name = strdup(name);
    char vert_name[128] = { 0 };
    char geom_name[128] = { 0 };
    char frag_name[128] = { 0 };
    strcpy(vert_name, name);
    strcpy(geom_name, name);
    strcpy(frag_name, name);
    strcat(vert_name, ".vert.glsl");
    strcat(geom_name, ".geom.glsl");
    strcat(frag_name, ".frag.glsl");

    char* tmp_shader_src;
    tmp_shader_src = fs_file_to_mem(vert_name);
    GLuint vert_shader = FScreateShader(GL_VERTEX_SHADER, tmp_shader_src);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);
    free(tmp_shader_src);
    if (!vert_shader) return NULL;

    GLuint geom_shader = 0;
    if (_access(geom_name, 0) != -1) {
        tmp_shader_src = fs_file_to_mem(geom_name);
        geom_shader = FScreateShader(GL_GEOMETRY_SHADER, tmp_shader_src);
        err = glGetError();
        if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);
        free(tmp_shader_src);
        if (!geom_shader) return NULL;
    }

    tmp_shader_src = fs_file_to_mem(frag_name);
    GLuint frag_shader = FScreateShader(GL_FRAGMENT_SHADER, tmp_shader_src);
    err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);
    free(tmp_shader_src);
    if (!frag_shader) return NULL;

    target_data->shader_program = glCreateProgram();
    glAttachShader(target_data->shader_program, vert_shader);
    glAttachShader(target_data->shader_program, frag_shader);
    if (geom_shader) glAttachShader(target_data->shader_program, geom_shader);
    err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);
    glBindFragDataLocation(target_data->shader_program, 0, "out_Color0");
    err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);
    glLinkProgram(target_data->shader_program);
    err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    if (geom_shader) glDeleteShader(geom_shader);
    err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);

	return target;
}

node* FSnode_unsetShader(node* target) {
    NULL_CHECK(target);
    fs_3d_data* target_data = (fs_3d_data*)target->data;
    NULL_CHECK(target_data);

    free(target_data->shader_name);
    target_data->shader_name = NULL;
    glDeleteProgram(target_data->shader_program);

    return target;
}
