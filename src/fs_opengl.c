#include "fs_opengl.h"

void GLAPIENTRY MessageCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar* message,
                                const void* userParam ) {
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

GLuint FScreateShader(GLenum shader_type, char* src) {
    GLuint tmp_sha = glCreateShader(shader_type);
    GLint status;
    char err_buf[4096], shader_type_str[16];
    switch (shader_type) {
        case GL_VERTEX_SHADER:          strcpy_s(&shader_type_str, 16, "Vertex ");    break;
        case GL_FRAGMENT_SHADER:        strcpy_s(&shader_type_str, 16, "Fragment ");  break;
        case GL_GEOMETRY_SHADER:        strcpy_s(&shader_type_str, 16, "Geometry ");  break;
        case GL_TESS_CONTROL_SHADER:    strcpy_s(&shader_type_str, 16, "Tess-Ctrl "); break;
        case GL_TESS_EVALUATION_SHADER: strcpy_s(&shader_type_str, 16, "Tess-Eval "); break;
        case GL_COMPUTE_SHADER:         strcpy_s(&shader_type_str, 16, "Compute ");   break;
        default: memset(&shader_type_str, 0, sizeof(shader_type_str));                break;
    }

    glShaderSource(tmp_sha, 1, &src, NULL);
    glCompileShader(tmp_sha);
    glGetShaderiv(tmp_sha, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(tmp_sha, sizeof(err_buf), NULL, err_buf);
        err_buf[sizeof(err_buf)-1] = '\0';
        fprintf(stderr, "%sShader compilation failed: %s\n", shader_type_str, err_buf);
        return -1;
    }

    return tmp_sha;
}

GLuint FScreateTexture(int size, unsigned char* data) {
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return tex_id;
}

GLuint FSgenerateTextureFBO(GLint target_fbo, int w, int h, TextureInfo *info) {
    GLint current_fbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_fbo);

    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0,
                 info->i_format, w, h, 0, info->format, info->type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (target_fbo && info->attachment)
        glFramebufferTexture2D(GL_FRAMEBUFFER, info->attachment, GL_TEXTURE_2D, tex_id, 0);
    info->texture_id = tex_id;

    glBindFramebuffer(GL_FRAMEBUFFER, current_fbo);
    return tex_id;
}