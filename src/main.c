#include <SDL2/SDL.h>
#include <Python/Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

// #if !defined(__glew_h__)
//     #include <GL/glew.h>
// #endif
#ifndef GLEW_STATIC
    #define GLEW_STATIC
#endif

#include "fs_stdfunc.h"
#include "fs_opengl.h"
// #include "fs_datatype.h"
#include "fs_py_loader_obj.h"

#ifdef _WIN32
    #include "windows.h"
#endif

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_gl3.h"

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

char* path[65535] = { 0 };
char* path_python[65535] = { 0 };
char* path_python_addon[65535] = { 0 };
char* path_python_script[65535] = { 0 };

TextureInfo FS_CoreFrameBufferTexture[] = {
    {
        .i_format   = GL_RGBA8,
        .format     = GL_RGBA,
        .type       = GL_UNSIGNED_BYTE,
        .attachment = GL_COLOR_ATTACHMENT0,
        .texture_id = 0,
    },
    {
        .i_format   = GL_RGBA8,
        .format     = GL_RGBA,
        .type       = GL_UNSIGNED_BYTE,
        .attachment = GL_COLOR_ATTACHMENT1,
        .texture_id = 0,
    },
    {
        .i_format   = GL_RGBA8,
        .format     = GL_RGBA,
        .type       = GL_UNSIGNED_BYTE,
        .attachment = GL_COLOR_ATTACHMENT2,
        .texture_id = 0,
    },
    {
        .i_format   = GL_DEPTH_COMPONENT32,
        .format     = GL_DEPTH_COMPONENT,
        .type       = GL_FLOAT,
        .attachment = GL_DEPTH_ATTACHMENT,
        .texture_id = 0,
    },
};

GLfloat FS_CoreScreenQuadVert[] = {
    -1.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  0.0f,
     1.0f,  1.0f,  0.0f,

     1.0f,  1.0f,  0.0f,
    -1.0f,  1.0f,  0.0f,
    -1.0f, -1.0f,  0.0f,
};
GLfloat FS_CoreScreenQuadTexCoord[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
};
GLuint FS_CoreScreenQuadVAO;
GLuint FS_CoreScreenQuadVBO;

GLuint FS_CoreScreenVertShader;
GLuint FS_CoreScreenFragShader;
GLuint FS_CoreScreenShaderProgram;

const int FPS_LIMIT = 60;
int current_resolution_x = 800;
int current_resolution_y = 600;

int teapot_vert_size, teapot_index_size;
float* teapot_vertex_array;
int* teapot_index_array;

GLuint FS_CoreFrameBuffer;

SDL_Window    *m_window;
SDL_GLContext  m_context;
GLuint         m_vao, m_vbo, m_ebo, m_tex;
GLuint         m_vert_shader;
GLuint         m_frag_shader;
GLuint         m_shader_prog;

bool mode_multiple_viewport = false;
bool mode_wireframe = false;
bool mode_fullscreen = false;
bool mode_console = false;

struct nk_context *ctx;
struct nk_colorf bg;
struct nk_font_atlas *atlas;

int Initialize();
int OGL_render_update();
int FS_clean_up();

void console_clear() {
    printf("                                           \r");
}

/*
 * Basic Initialization
 */
int Initialize() {
    printf("Initializing...\n");

    // Initialize SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL video\n");
        return 1;
    }

    // Create main window
    int sdl_window_status = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    m_window = SDL_CreateWindow(
        "FoxSnow",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        current_resolution_x, current_resolution_y,
        sdl_window_status);

    if (m_window == NULL) {
        fprintf(stderr, "Failed to create main window\n");
        SDL_Quit();
        return 1;
    }

    // Initialize rendering context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_context = SDL_GL_CreateContext(m_window);
    if (m_context == NULL) {
        fprintf(stderr, "Failed to create GL context\n");
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 1;
    }

    // SDL_GL_SetSwapInterval(1); // Use VSYNC

    // // Initialize GL Extension Wrangler (GLEW)
    // glewExperimental = GL_TRUE; // Please expose OpenGL 3.x+ interfaces
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to init GLEW\n");
        SDL_GL_DeleteContext(m_context);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 1;
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glFrontFace(GL_CCW);
    glDisable(GL_DEPTH_CLAMP);

    #ifndef NODEBUG
        // During init, enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);
    #endif

    /* Initialize Shaders */
    GLint status;
    char err_buf[4096];

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    get_exe_path(path, 65535);
    char* tmp = str_replace(path, "foxsnow.exe", "");
    memset(path, 0, sizeof(path));
    strcpy_s(path, 65535, tmp);
    free(tmp);
    strcpy_s(path_python, 65535, path);
    strcat(path_python, "Python37");
    strcpy_s(path_python_addon, 65535, path);
    strcat(path_python_addon, "Python_addon");
    strcpy_s(path_python_script, 65535, path);
    strcat(path_python_script, "Python_src");

    SDL_setenv("PYTHONHOME", path_python, true);
    SDL_setenv("PYTHONPATH", path_python, true);
    Py_Initialize();

    PyObject *sys = PyImport_ImportModule("sys");
    PyObject *path = PyObject_GetAttrString(sys, "path");
    PyList_Append(path, PyUnicode_FromString(path_python_addon));
    PyList_Append(path, PyUnicode_FromString(path_python_script));

    PyRun_SimpleString("import time;import numpy;print(numpy.version.version)");
    if (FS_PyConsole_init()) {
        fprintf(stderr, "Initialize Python Console Failed\n");
    }

    char* vert_shader_src = file_to_mem("default.vert.glsl");
    m_vert_shader = FScreateShader(GL_VERTEX_SHADER, vert_shader_src);
    free(vert_shader_src);
    if (m_vert_shader == -1) return 1;

    char* frag_shader_src = file_to_mem("default.frag.glsl");
    m_frag_shader = FScreateShader(GL_FRAGMENT_SHADER, frag_shader_src);
    free(frag_shader_src);
    if (m_frag_shader == -1) return 1;

    m_shader_prog = glCreateProgram();
    glAttachShader(m_shader_prog, m_vert_shader);
    glAttachShader(m_shader_prog, m_frag_shader);
    glBindFragDataLocation(m_shader_prog, 0, "out_Color0");
    glLinkProgram(m_shader_prog);
    glUseProgram(m_shader_prog);

    FSloadOBJ("resources/teapot.obj", &teapot_vert_size,  &teapot_vertex_array,
                                      &teapot_index_size, &teapot_index_array);
    printf("index  | size = %d, pointer = %p | AT C\n", teapot_vert_size, teapot_index_array);
    printf("result = %lf\n", teapot_vertex_array[100]);
    printf("result = %d\n", teapot_index_array[100]);

    /* Initialize Geometry */
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, teapot_vert_size*sizeof(float), teapot_vertex_array, GL_STATIC_DRAW);

    // Populate element buffer
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, teapot_index_size*sizeof(int), teapot_index_array, GL_STATIC_DRAW);


    // Bind vertex position attribute
    GLint pos_attr_loc = glGetAttribLocation(m_shader_prog, "fs_Vertex");
    glEnableVertexAttribArray(pos_attr_loc);
    glVertexAttribPointer(pos_attr_loc, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* FBO - Create FBO */
    glGenFramebuffers(1, &FS_CoreFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, FS_CoreFrameBuffer);

    /* FBO - Generate Texture */
    for (int z=0; z < 4; z++)
        FSgenerateTextureFBO(
            FS_CoreFrameBuffer,
            current_resolution_x,
            current_resolution_y,
            &FS_CoreFrameBufferTexture[z]);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("FS GL : FRAMEBUFFER NOT COMPLETE");
        exit(1);
    }

    GLenum tmp_buf[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
    };
    glDrawBuffers(3, tmp_buf);

    /* FBO - Setup Shader */
    vert_shader_src = file_to_mem("core_screen.vert.glsl");
    FS_CoreScreenVertShader = FScreateShader(GL_VERTEX_SHADER, vert_shader_src);
    free(vert_shader_src);
    if (FS_CoreScreenVertShader == -1) return 1;

    frag_shader_src = file_to_mem("core_screen.frag.glsl");
    FS_CoreScreenFragShader = FScreateShader(GL_FRAGMENT_SHADER, frag_shader_src);
    free(frag_shader_src);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(FS_CoreScreenQuadVert)+sizeof(FS_CoreScreenQuadTexCoord), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(FS_CoreScreenQuadVert), FS_CoreScreenQuadVert);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(FS_CoreScreenQuadVert), sizeof(FS_CoreScreenQuadTexCoord), FS_CoreScreenQuadTexCoord);

    pos_attr_loc = glGetAttribLocation(FS_CoreScreenShaderProgram, "fs_Vertex");
    glVertexAttribPointer(pos_attr_loc, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(pos_attr_loc);
    
    // err = glGetError();
    // if (err != GL_NO_ERROR) printf("ERROR - glError Y: 0x%04X\n", err);

    pos_attr_loc = glGetAttribLocation(FS_CoreScreenShaderProgram, "fs_MultiTexCoord0");
    glEnableVertexAttribArray(pos_attr_loc);
    glVertexAttribPointer(pos_attr_loc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (void*)sizeof(FS_CoreScreenQuadVert));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // err = glGetError();
    // if (err != GL_NO_ERROR) printf("ERROR - glError Z: 0x%04X\n", err);

    // Bind vertex texture coordinate attribute
    // GLint tex_attr_loc = glGetAttribLocation(m_shader_prog, "fs_MultiTexCoord0");
    // glVertexAttribPointer(tex_attr_loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
    // glEnableVertexAttribArray(tex_attr_loc);

    // /* Initialize textures */
    // GLuint screen_tex = FScreateTexture(256, logo_rgba);
    // glUniform1i(glGetUniformLocation(m_shader_prog, "tex_1"), 0);


    GLenum err = glGetError();
    if (err != GL_NO_ERROR) printf("ERROR - glError 8: 0x%04X\n", err);

    ctx = nk_sdl_init(m_window);
    {
        nk_sdl_font_stash_begin(&atlas);
        struct nk_font *roboto  = nk_font_atlas_add_from_file(atlas, "./resources/font/Roboto-Regular.ttf", 16, 0);
        nk_sdl_font_stash_end();
        
        //nk_style_load_all_cursors(ctx, atlas->cursors);
        nk_style_set_font(ctx, &roboto->handle);

        ctx->style.window.background = nk_rgba(38,38,38,128);
        ctx->style.window.header.normal = nk_style_item_color(nk_rgba(38,38,38,128));
        ctx->style.window.header.hover  = nk_style_item_color(nk_rgba(38,38,38,192));
        ctx->style.window.header.active = nk_style_item_color(nk_rgba(38,38,38,224));
        ctx->style.window.header.minimize_button.normal = nk_style_item_color(nk_rgba(38,38,38,0));
        ctx->style.window.header.minimize_button.hover  = nk_style_item_color(nk_rgba(38,38,38,0));
        ctx->style.window.header.minimize_button.active = nk_style_item_color(nk_rgba(38,38,38,0));
        ctx->style.window.header.minimize_button.border_color = nk_rgba(38,38,38,224);
        ctx->style.window.header.label_active = nk_rgb(212,212,212);
        ctx->style.window.fixed_background = nk_style_item_color(nk_rgba(38,38,38,192));
        ctx->style.window.border_color = nk_rgb(18,86,133);
        // ctx->style.window.contextual_border_color = nk_rgb(255,165,0);
        // ctx->style.window.menu_border_color = nk_rgb(255,165,0);
        // ctx->style.window.group_border_color = nk_rgb(255,165,0);
        // ctx->style.window.tooltip_border_color = nk_rgb(255,165,0);
        // ctx->style.window.scrollbar_size = nk_vec2(16,16);
        // ctx->style.window.border_color = nk_rgba(0,0,0,0);
        // ctx->style.window.border = 1;

        ctx->style.button.normal = nk_style_item_color(nk_rgba(38,38,38,0));
        ctx->style.button.hover = nk_style_item_color(nk_rgba(38,38,38,192));
        ctx->style.button.border_color = nk_rgb(0,122,204);
        ctx->style.button.text_normal = nk_rgb(212,212,212);
        ctx->style.button.text_hover = nk_rgb(212,212,212);
        ctx->style.button.text_active = nk_rgb(212,212,212);

        ctx->style.text.color = nk_rgb(212,212,212);

        ctx->style.edit.normal = nk_style_item_color(nk_rgba(38,38,38,192));
        ctx->style.edit.hover  = nk_style_item_color(nk_rgba(38,38,38,192));
        ctx->style.edit.active = nk_style_item_color(nk_rgba(38,38,38,192));
        ctx->style.edit.border_color = nk_rgb(18,86,133);
        ctx->style.edit.text_normal = nk_rgb(212,212,212);
        ctx->style.edit.text_hover  = nk_rgb(212,212,212);
        ctx->style.edit.text_active = nk_rgb(212,212,212);
        ctx->style.edit.selected_normal = nk_rgb(212,212,212);
        ctx->style.edit.selected_hover  = nk_rgb(212,212,212);
        ctx->style.edit.selected_text_normal = nk_rgb(212,212,212);
        ctx->style.edit.selected_text_hover  = nk_rgb(212,212,212);
        // ctx->style.button.active = nk_style_item_color(nk_rgb(220,10,0));
        // ctx->style.button.text_background = nk_rgb(0,0,0);
    }

    return 0;
}

/*
 * Free resources
 */
int FS_clean_up() {
    printf("Exiting...\n");

    if (Py_FinalizeEx() < 0) return 120;

    nk_sdl_shutdown();

    glUseProgram(0);
    glDisableVertexAttribArray(0);
    glDetachShader(m_shader_prog, m_vert_shader);
    glDetachShader(m_shader_prog, m_frag_shader);
    glDeleteProgram(m_shader_prog);
    glDeleteShader(m_vert_shader);
    glDeleteShader(m_frag_shader);
    glDeleteTextures(1, &m_tex);
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);

    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();

    return 0;
}

/*
 * Render a frame
 */
int OGL_render_update() {
    glPolygonMode(GL_FRONT_AND_BACK, (mode_wireframe ? GL_LINE : GL_FILL));
    glBindFramebuffer(GL_FRAMEBUFFER, FS_CoreFrameBuffer);

    glClearColor(0.5f, 0.75f, 1.0f, 1.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glUseProgram(m_shader_prog);
    glDrawElements(GL_TRIANGLES, teapot_index_size, GL_UNSIGNED_INT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glBindVertexArray(FS_CoreScreenQuadVAO);
    glUseProgram(FS_CoreScreenShaderProgram);

    if (mode_multiple_viewport) {
        int multi_scrn_tgt_x = current_resolution_x/3;
        int multi_scrn_tgt_y = current_resolution_y/3;

        // Main
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[0].texture_id);
        glViewport(0, multi_scrn_tgt_y+1, multi_scrn_tgt_x*2+1, multi_scrn_tgt_y*2);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 1X3
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[0].texture_id);
        glViewport(multi_scrn_tgt_x*2+2, multi_scrn_tgt_y*2+2, multi_scrn_tgt_x, multi_scrn_tgt_y);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 2X3
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[1].texture_id);
        glViewport(multi_scrn_tgt_x*2+2, multi_scrn_tgt_y+1, multi_scrn_tgt_x, multi_scrn_tgt_y);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 3X3
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[2].texture_id);
        glViewport(multi_scrn_tgt_x*2+2, 0, multi_scrn_tgt_x, multi_scrn_tgt_y);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 3X1
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[3].texture_id);
        glViewport(0, 0, multi_scrn_tgt_x, multi_scrn_tgt_y);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 3X2
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[0].texture_id);
        glViewport(multi_scrn_tgt_x+1, 0, multi_scrn_tgt_x, multi_scrn_tgt_y);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    } else {
        // Main only
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[0].texture_id);
        glViewport(0, 0, current_resolution_x, current_resolution_y);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) printf("ERROR WHILE RENDER - glError : 0x%04X\n", err);

    nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

    SDL_GL_SwapWindow(m_window);
    return 0;
}

void SDL_onResize(int w, int h) {
    if (!w || !h)
        SDL_GetWindowSize(m_window, &current_resolution_x, &current_resolution_y);
    else
        current_resolution_x = w, current_resolution_y = h;

    glDeleteFramebuffers(1, &FS_CoreFrameBuffer);
     for(int z=0;z<4;z++) {
         glDeleteTextures(1, &FS_CoreFrameBufferTexture[z].texture_id);
    }

    glGenFramebuffers(1, &FS_CoreFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, FS_CoreFrameBuffer);

    /* FBO - Generate Texture */
    for (int z=0; z < 4; z++)
        FSgenerateTextureFBO(
            FS_CoreFrameBuffer,
            current_resolution_x,
            current_resolution_y,
            &FS_CoreFrameBufferTexture[z]);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("FS GL : FRAMEBUFFER NOT COMPLETE");
        exit(1);
    }

    console_clear();
    printf("FS SDL : EVENT RESIZE %d %d\n",
           current_resolution_x, current_resolution_y);
}

int main(int argc, char *argv[]) {
    bool should_run = true;
    double fps = 0.0f;
    unsigned long long frame = 0, start_tick = 0;

    if (Initialize()) return 1;

    printf("Running...\n");
    while(should_run) {
        start_tick = SDL_GetTicks();

        bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
        SDL_Event event;
        nk_input_begin(ctx);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                should_run = false;
                break;
            }
            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_onResize(event.window.data1, event.window.data2);
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_BACKQUOTE) mode_console = !mode_console;
                
                if (!mode_console) {
                    if (event.key.keysym.sym == SDLK_w) mode_wireframe  = !mode_wireframe;
                    else if (event.key.keysym.sym == SDLK_f) {
                        // SDL_WINDOW_FULLSCREEN will do 'real' fullscreen with video mode change,
                        // while SDL_WINDOW_FULLSCREEN_DESKTOP will do "fake" fullscreen
                        // that takes the size of the desktop.
                        mode_fullscreen = !mode_fullscreen;

                        SDL_SetWindowFullscreen(m_window, (mode_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
                        SDL_onResize(0, 0);
                    }
                    else if (event.key.keysym.sym == SDLK_TAB) mode_multiple_viewport = !mode_multiple_viewport;
                    else if (event.key.keysym.sym == SDLK_ESCAPE) should_run = false;
                }
            }
            nk_sdl_handle_event(&event);
        }
        nk_input_end(ctx);

        if (mode_console) {
            if (nk_begin(ctx, "Interactive Console", nk_rect(30, 70, 480, 320),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {

                static const float ratio[] = {0.9f, 0.1f};
                static char box_buffer[INT_MAX];
                static int box_len = 0;
                static char text[128];
                static int text_len;
                nk_flags active;
                float window_height = nk_window_get_height(ctx);
                float window_width  = nk_window_get_width(ctx);

                nk_layout_row_dynamic(ctx, window_height - 90, 1);
                nk_edit_string(ctx, NK_EDIT_BOX, box_buffer, &box_len, 512, nk_filter_default);

                nk_layout_row_begin(ctx, NK_STATIC, 25, 3);
                nk_layout_row_push(ctx, 20);
                nk_label(ctx, ">>>", NK_TEXT_LEFT);

                nk_layout_row_push(ctx, window_width - 125);
                active = nk_edit_string(ctx, NK_EDIT_FIELD|NK_EDIT_SIG_ENTER, text, &text_len, 64,  nk_filter_ascii);

                nk_layout_row_push(ctx, 69);
                if (nk_button_label(ctx, "Submit") || (active & NK_EDIT_COMMITED)) {
                    text[text_len] = '\n';
                    text_len++;
                    memcpy(&box_buffer[box_len], &text, (nk_size)text_len);
                    box_len += text_len;
                    text_len = 0;
                }
                
                nk_layout_row_end(ctx);

            }
            nk_end(ctx);
        }

        OGL_render_update();

        if (1000.0f / FPS_LIMIT > SDL_GetTicks() - start_tick)
            SDL_Delay(1000.0f / FPS_LIMIT - (float)(SDL_GetTicks() - start_tick));

        fps = 1000.0f / (float)(SDL_GetTicks() - start_tick);
        frame++;

        printf("FPS : %15lf, Frame : %llu\r", fps, frame);
    }

    printf("\n");
    return FS_clean_up();
}