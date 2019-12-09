#ifndef FS_OPENGL
    #define FS_OPENGL

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>

    #ifndef GLEW_STATIC
        #define GLEW_STATIC
    #endif
    #ifndef __glew_h__
        #include <GL/glew.h>
    #endif

    #include "../datatype/fs_node.h"
    #include "../loader/3d_obj.h"

    #define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

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

    matrix* FS_ViewMatrix;
    matrix* FS_ProjectionMatrix;

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
    node* FSloadModel(node* target, char* model, char* vert, char* frag);

    int FS_GLscreenInit(int resolution_x, int resolution_y);
#endif