#include "fs_opengl.h"

extern FS_GL_VERSION FS_GL_VERSION_LIST[] = {
    {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0},
    {3, 3}, {3, 2}, {3, 1}, {3, 0},
    {2, 1}, {2, 0}
};
FS_GL_VERSION_LIST_LEN = COUNT_OF(FS_GL_VERSION_LIST);

TextureInfo FS_CoreFrameBufferTexture[4] = {
    {
        .i_format = GL_RGBA8,
        .format = GL_RGBA,
        .type = GL_UNSIGNED_BYTE,
        .attachment = GL_COLOR_ATTACHMENT0,
        .texture_id = 0,

        .width = 0,
        .height = 0,

        .wrap_s = GL_CLAMP_TO_BORDER,
        .wrap_t = GL_CLAMP_TO_BORDER,

        .filter_min = GL_LINEAR,
        .filter_mag = GL_LINEAR,
    },
    {
        .i_format = GL_RGBA8,
        .format = GL_RGBA,
        .type = GL_UNSIGNED_BYTE,
        .attachment = GL_COLOR_ATTACHMENT1,
        .texture_id = 0,

        .width = 0,
        .height = 0,

        .wrap_s = GL_CLAMP_TO_BORDER,
        .wrap_t = GL_CLAMP_TO_BORDER,

        .filter_min = GL_LINEAR,
        .filter_mag = GL_LINEAR,
    },
    {
        .i_format = GL_RGBA8,
        .format = GL_RGBA,
        .type = GL_UNSIGNED_BYTE,
        .attachment = GL_COLOR_ATTACHMENT2,
        .texture_id = 0,

        .width = 0,
        .height = 0,

        .wrap_s = GL_CLAMP_TO_BORDER,
        .wrap_t = GL_CLAMP_TO_BORDER,

        .filter_min = GL_LINEAR,
        .filter_mag = GL_LINEAR,
    },
    {
        .i_format = GL_DEPTH_COMPONENT32,
        .format = GL_DEPTH_COMPONENT,
        .type = GL_FLOAT,
        .attachment = GL_DEPTH_ATTACHMENT,
        .texture_id = 0,

        .width = 0,
        .height = 0,

        .wrap_s = GL_CLAMP_TO_BORDER,
        .wrap_t = GL_CLAMP_TO_BORDER,

        .filter_min = GL_LINEAR,
        .filter_mag = GL_LINEAR,
    },
};

const GLfloat FS_CoreScreenQuadVert[18] = {
    -1.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  0.0f,
     1.0f,  1.0f,  0.0f,

     1.0f,  1.0f,  0.0f,
    -1.0f,  1.0f,  0.0f,
    -1.0f, -1.0f,  0.0f,
};
const GLfloat FS_CoreScreenQuadTexCoord[12] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
};

void GLAPIENTRY MessageCallback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}

GLuint FScreateShader(GLenum shader_type, char* src) {
    GLuint tmp_sha = glCreateShader(shader_type);
    GLint status;
    char err_buf[4096] = { 0 }, shader_type_str[16] = { 0 };
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
        err_buf[sizeof(err_buf) - 1] = '\0';
        fprintf(stderr, "%sShader compilation failed: %s\n", shader_type_str, err_buf);
        return 0;
    }

    return tmp_sha;
}

GLuint FScreateTexture(int size, unsigned char* data) {
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return tex_id;
}

GLuint FSgenerateTextureFBO(GLint target_fbo, int w, int h, TextureInfo* info) {
    GLint current_fbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_fbo);
    info->width = w;
    info->height = h;

    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0,
        info->i_format, w, h, 0, info->format, info->type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, info->filter_min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, info->filter_mag);
    if (target_fbo && info->attachment)
        glFramebufferTexture2D(GL_FRAMEBUFFER, info->attachment, GL_TEXTURE_2D, tex_id, 0);
    info->texture_id = tex_id;

    glBindFramebuffer(GL_FRAMEBUFFER, current_fbo);
    return tex_id;
}

int FS_GLscreenInit(int resolution_x, int resolution_y) {
    GLint tmp_attr_location;
    char* shader_src = NULL;

    /* FBO - Create FBO */
    glGenFramebuffers(1, &FS_CoreFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, FS_CoreFrameBuffer);

    /* FBO - Generate Texture */
    for (int z = 0; z < 4; z++)
        FSgenerateTextureFBO(
            FS_CoreFrameBuffer,
            resolution_x, resolution_y,
            &FS_CoreFrameBufferTexture[z]);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        eprint("FS GL : FRAMEBUFFER NOT COMPLETE");
        return 1;
    }

    GLenum tmp_buf[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
    };
    glDrawBuffers(3, tmp_buf);

    /* FBO - Setup Shader */
    // Vertex shader
    shader_src = fs_file_to_mem("core_screen.vert.glsl");
    FS_CoreScreenVertShader = FScreateShader(GL_VERTEX_SHADER, shader_src);
    free(shader_src);
    if (FS_CoreScreenVertShader == -1) return 1;

    // Fragment shader
    shader_src = fs_file_to_mem("core_screen.frag.glsl");
    FS_CoreScreenFragShader = FScreateShader(GL_FRAGMENT_SHADER, shader_src);
    free(shader_src);
    if (FS_CoreScreenFragShader == -1) return 1;

    FS_CoreScreenShaderProgram = glCreateProgram();
    glAttachShader(FS_CoreScreenShaderProgram, FS_CoreScreenVertShader);
    glAttachShader(FS_CoreScreenShaderProgram, FS_CoreScreenFragShader);
    glBindFragDataLocation(FS_CoreScreenShaderProgram, 0, "out_Color0");
    glLinkProgram(FS_CoreScreenShaderProgram);
    glUseProgram(FS_CoreScreenShaderProgram);

    /* FBO - Initialize Geometry */
    glGenVertexArrays(1, &FS_CoreScreenQuadVAO);
    glBindVertexArray(FS_CoreScreenQuadVAO);
    glGenBuffers(1, &FS_CoreScreenQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, FS_CoreScreenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(FS_CoreScreenQuadVert) + sizeof(FS_CoreScreenQuadTexCoord), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(FS_CoreScreenQuadVert), FS_CoreScreenQuadVert);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(FS_CoreScreenQuadVert), sizeof(FS_CoreScreenQuadTexCoord), FS_CoreScreenQuadTexCoord);

    tmp_attr_location = glGetAttribLocation(FS_CoreScreenShaderProgram, "fs_Vertex");
    glVertexAttribPointer(tmp_attr_location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(tmp_attr_location);

    tmp_attr_location = glGetAttribLocation(FS_CoreScreenShaderProgram, "fs_MultiTexCoord0");
    glEnableVertexAttribArray(tmp_attr_location);
    glVertexAttribPointer(tmp_attr_location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)sizeof(FS_CoreScreenQuadVert));


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Bind vertex texture coordinate attribute
    // GLint tex_attr_loc = glGetAttribLocation(m_shader_prog, "fs_MultiTexCoord0");
    // glVertexAttribPointer(tex_attr_loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
    // glEnableVertexAttribArray(tex_attr_loc);

    // /* Initialize textures */
    // GLuint screen_tex = FScreateTexture(256, logo_rgba);
    // glUniform1i(glGetUniformLocation(m_shader_prog, "tex_1"), 0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);
    return 0;
}

void FScalculateViewFrustum(
        matrix* output,
        const float left, const float right,
        const float bottom, const float top,
        const float near_val, const float far_val) {
    if ((right - left) == 0.0f || (top - bottom) == 0.0f || (far_val - near_val) == 0.0f)
        return;

    output->mat[0] = 2.0f * near_val / (right - left);
    output->mat[5] = 2.0f * near_val / (top - bottom);
    output->mat[8] = (right + left) / (right - left);
    output->mat[9] = (top + bottom) / (top - bottom);
    output->mat[10] = -(far_val + near_val) / (far_val - near_val);
    output->mat[11] = -1.0f;
    output->mat[14] = -(2.0f * far_val * near_val) / (far_val - near_val);

    output->mat[1] = output->mat[2] = output->mat[3] = output->mat[4] =
    output->mat[6] = output->mat[7] = output->mat[12] = output->mat[13] = output->mat[15] = 0.0f;
}

void FScalculatePerspectiveMatrix(matrix* x, const float fovy, const float aspect, const float z_near, const float z_far) {
    if (fovy <= 0.0f || fovy >= 180.0f) return;

    float xmin, xmax, ymin, ymax;
    ymax = z_near * tanf(fovy * M_PI / 360.0f);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;
    FScalculateViewFrustum(x, xmin, xmax, ymin, ymax, z_near, z_far);
}
