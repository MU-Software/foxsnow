#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// #if !defined(__glew_h__)
//     #include <GL/glew.h>
// #endif
#ifndef GLEW_STATIC
    #define GLEW_STATIC
#endif

#include "fs_datatype.h"

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

typedef struct _FS_GL_VERSION {
    int major;
    int minor;
} FS_GL_VERSION;

extern FS_GL_VERSION FS_GL_VERSION_LIST[];
int FS_GL_VERSION_LIST_LEN;

void GLAPIENTRY MessageCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar* message,
                                const void* userParam );
GLuint FScreateShader(GLenum shader_type, char* src);
GLuint FScreateTexture(int size, unsigned char* data);
GLuint FSgenerateTextureFBO(GLint target_fbo, int w, int h, TextureInfo *info);