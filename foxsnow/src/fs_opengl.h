#ifndef FS_LIB_SUPPORT_OPENGL
#define FS_LIB_SUPPORT_OPENGL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define _USE_MATH_DEFINES
#include <math.h>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#ifndef __glew_h__
#include <GL/glew.h>
#endif

#include "fs_stdfunc.h"
#include "datatype/fs_matrix.h"

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

extern unsigned long long frame_number;
extern int current_resolution_x;
extern int current_resolution_y;

typedef struct _FS_GL_VERSION {
    int major;
    int minor;
} FS_GL_VERSION;

extern FS_GL_VERSION FS_GL_VERSION_LIST[];
int FS_GL_VERSION_LIST_LEN;

typedef struct _TextureInfo {
    GLint i_format;
    GLint format;
    GLenum type;
    GLenum attachment;
    GLuint texture_id;

    int width;
    int height;

    GLuint wrap_s;
    GLuint wrap_t;

    GLuint filter_min;
    GLuint filter_mag;  // mag is not a typo
} TextureInfo;

extern const GLfloat FS_CoreScreenQuadVert[18];
extern const GLfloat FS_CoreScreenQuadTexCoord[12];

extern TextureInfo FS_CoreFrameBufferTexture[4];
GLuint FS_CoreScreenQuadVAO;
GLuint FS_CoreScreenQuadVBO;

GLuint FS_CoreScreenVertShader;
GLuint FS_CoreScreenFragShader;
GLuint FS_CoreScreenShaderProgram;

GLuint FS_CoreFrameBuffer;

void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam);
GLuint FScreateShader(GLenum shader_type, char* src);
GLuint FScreateTexture(int size, unsigned char* data);
GLuint FSgenerateTextureFBO(GLint target_fbo, int w, int h, TextureInfo* info);

int FS_GLscreenInit(int resolution_x, int resolution_y);
void FScalculateViewFrustum(
    matrix* output,
    const float left, const float right,
    const float bottom, const float top,
    const float near_val, const float far_val);
void FScalculatePerspectiveMatrix(matrix* x, const float fovy, const float aspect, const float z_near, const float z_far);
#endif