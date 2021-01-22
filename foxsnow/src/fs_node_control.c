#include "fs_node_control.h"

node* FSnode_setShader(node* target, const char* name) {
    NULL_CHECK(target);
    NULL_CHECK(name);
    fs_3d_data* target_data = (fs_3d_data*)target->data;
    NULL_CHECK(target_data);

    unsigned int shader_name_hash = hash(name);
    rbtree_node* cache_result = rbtree_search(cached_shader, shader_name_hash);
    if (cache_result) {
        target_data->shader = cache_result;
        ((FS_Type_ShaderInfo*)cache_result->data)->reference_count++;
        return target;
    }

    FS_Type_ShaderInfo* new_shader = (FS_Type_ShaderInfo*)calloc(1, sizeof(FS_Type_ShaderInfo));
    ALLOC_FAILCHECK(new_shader);
    rbtree_insert(&cached_shader, shader_name_hash, (void*)new_shader, strdup("FS_Type_ShaderInfo"));
    rbtree_node* new_shader_node = rbtree_search(cached_shader, shader_name_hash);
    new_shader->reference_count++;
    new_shader->shader_name = strdup(name);
    
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


    new_shader->shader_program = glCreateProgram();
    glAttachShader(new_shader->shader_program, vert_shader);
    glAttachShader(new_shader->shader_program, frag_shader);
    if (geom_shader) glAttachShader(new_shader->shader_program, geom_shader);
    err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);
    glBindFragDataLocation(new_shader->shader_program, 0, "out_Color0");
    err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);
    glLinkProgram(new_shader->shader_program);
    err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    if (geom_shader) glDeleteShader(geom_shader);
    err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);

    target_data->shader = new_shader_node;
	return target;
}

node* FSnode_unsetShader(node* target) {
    NULL_CHECK(target);
    fs_3d_data* target_data = (fs_3d_data*)target->data;
    NULL_CHECK(target_data);
    rbtree_node* target_shader_node = (rbtree_node*)target_data->shader;
    FS_Type_ShaderInfo* target_shader = (FS_Type_ShaderInfo*)target_shader_node->data;
    NULL_CHECK(target_shader);

    target_shader->reference_count--;
    target_data->shader = NULL;

    // TODO: Remove shader from rbtree!

    return target;
}
