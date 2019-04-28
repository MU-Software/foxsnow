#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#if !defined(__glew_h__)
    #include <GL/glew.h>
#endif

typedef struct _TextureInfo {
    GLint i_format;
    GLint format;
    GLenum type;
    GLenum attachment;
    GLuint texture_id;
} TextureInfo;