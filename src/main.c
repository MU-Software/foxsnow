#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include <SDL2/SDL.h>
#include <Python/Python.h>

#ifndef GLEW_STATIC
    #define GLEW_STATIC
#endif
#ifndef __glew_h__
    #include <GL/glew.h>
#endif

#include "fs_stdfunc.h"
// #include "nuklear_support/nuklear_init.h"
#include "GL_support/fs_opengl.h"
#include "datatype/fs_datatype.h"
#include "python_support/console.h"
#include "python_support/loader_obj.h"
#include "loader/3d_obj.h"

    #define FS_NUKLEAR

    #ifndef GLEW_STATIC
        #define GLEW_STATIC
    #endif
    #ifndef __glew_h__
        #include <GL/glew.h>
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
    
    struct nk_context *fs_nk_context;
    struct nk_font_atlas *fs_nk_font_atlas;

    void FS_nk_consoleInit(SDL_Window* window) {
        fs_nk_context = nk_sdl_init(window);
        {
            nk_sdl_font_stash_begin(&fs_nk_font_atlas);
            struct nk_font *roboto  = nk_font_atlas_add_from_file(fs_nk_font_atlas, "./resources/font/Roboto-Regular.ttf", 16, 0);
            nk_sdl_font_stash_end();
            
            //nk_style_load_all_cursors(ctx, atlas->cursors);
            nk_style_set_font(fs_nk_context, &roboto->handle);

            fs_nk_context->style.window.background = nk_rgba(38,38,38,128);
            fs_nk_context->style.window.header.normal = nk_style_item_color(nk_rgba(38,38,38,128));
            fs_nk_context->style.window.header.hover  = nk_style_item_color(nk_rgba(38,38,38,192));
            fs_nk_context->style.window.header.active = nk_style_item_color(nk_rgba(38,38,38,224));
            fs_nk_context->style.window.header.minimize_button.normal = nk_style_item_color(nk_rgba(38,38,38,0));
            fs_nk_context->style.window.header.minimize_button.hover  = nk_style_item_color(nk_rgba(38,38,38,0));
            fs_nk_context->style.window.header.minimize_button.active = nk_style_item_color(nk_rgba(38,38,38,0));
            fs_nk_context->style.window.header.minimize_button.border_color = nk_rgba(38,38,38,224);
            fs_nk_context->style.window.header.label_active = nk_rgb(212,212,212);
            fs_nk_context->style.window.fixed_background = nk_style_item_color(nk_rgba(38,38,38,192));
            fs_nk_context->style.window.border_color = nk_rgb(18,86,133);

            fs_nk_context->style.button.normal = nk_style_item_color(nk_rgba(38,38,38,0));
            fs_nk_context->style.button.hover = nk_style_item_color(nk_rgba(38,38,38,192));
            fs_nk_context->style.button.border_color = nk_rgb(0,122,204);
            fs_nk_context->style.button.text_normal = nk_rgb(212,212,212);
            fs_nk_context->style.button.text_hover = nk_rgb(212,212,212);
            fs_nk_context->style.button.text_active = nk_rgb(212,212,212);

            fs_nk_context->style.text.color = nk_rgb(212,212,212);

            fs_nk_context->style.edit.normal = nk_style_item_color(nk_rgba(38,38,38,192));
            fs_nk_context->style.edit.hover  = nk_style_item_color(nk_rgba(38,38,38,192));
            fs_nk_context->style.edit.active = nk_style_item_color(nk_rgba(38,38,38,192));
            fs_nk_context->style.edit.border_color = nk_rgb(18,86,133);
            fs_nk_context->style.edit.text_normal = nk_rgb(212,212,212);
            fs_nk_context->style.edit.text_hover  = nk_rgb(212,212,212);
            fs_nk_context->style.edit.text_active = nk_rgb(212,212,212);
            fs_nk_context->style.edit.selected_normal = nk_rgb(212,212,212);
            fs_nk_context->style.edit.selected_hover  = nk_rgb(212,212,212);
            fs_nk_context->style.edit.selected_text_normal = nk_rgb(212,212,212);
            fs_nk_context->style.edit.selected_text_hover  = nk_rgb(212,212,212);
        }
    }





#ifdef _WIN32
    #include "windows.h"
#endif

char* path[65535] = { 0 };
char* path_python[65535] = { 0 };
char* path_python_addon[65535] = { 0 };
char* path_python_script[65535] = { 0 };


int TARGET_GL_MAJOR_VERSION = 0;
int TARGET_GL_MINOR_VERSION = 0;
bool NUKLEAR_ENABLE = false;
bool PYTHON_ENABLE = true;

const int FPS_LIMIT = 60;
int current_resolution_x = 800;
int current_resolution_y = 600;

int teapot_vert_size, teapot_index_size;
float* teapot_vertex_array;
int* teapot_index_array;

SDL_Window    *fs_sdl_window;
SDL_GLContext  fs_sdl_GLcontext;
GLuint         m_vao, m_vbo, m_ebo, m_tex;
GLuint         m_vert_shader;
GLuint         m_frag_shader;
GLuint         m_shader_prog;

bool mode_multiple_viewport = false;
bool mode_wireframe = false;
bool mode_fullscreen = false;
bool mode_console = false;

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
    fs_sdl_window = SDL_CreateWindow(
        "FoxSnow",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        current_resolution_x, current_resolution_y,
        sdl_window_status);

    if (fs_sdl_window == NULL) {
        fprintf(stderr, "Failed to create main window\n");
        SDL_Quit();
        return 1;
    }

    // Initialize rendering context
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    int TARGET_GL_VERSION_INDEX = 0;
    for(TARGET_GL_VERSION_INDEX; TARGET_GL_VERSION_INDEX <= FS_GL_VERSION_LIST_LEN; TARGET_GL_VERSION_INDEX++) {
        TARGET_GL_MAJOR_VERSION = FS_GL_VERSION_LIST[TARGET_GL_VERSION_INDEX].major;
        TARGET_GL_MINOR_VERSION = FS_GL_VERSION_LIST[TARGET_GL_VERSION_INDEX].minor;
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, TARGET_GL_MAJOR_VERSION);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, TARGET_GL_MINOR_VERSION);

        fs_sdl_GLcontext = SDL_GL_CreateContext(fs_sdl_window);
        if (fs_sdl_GLcontext == NULL) continue;
        else break;
    }
    if (fs_sdl_GLcontext == NULL) {
        fprintf(stderr, "Failed to create GL context(TOTAL_FAILURE)\n");
        SDL_DestroyWindow(fs_sdl_window);
        SDL_Quit();
        return 1;
    }
    printf("Created GL context successfully on GL %d.%d.\n",
           TARGET_GL_MAJOR_VERSION, TARGET_GL_MINOR_VERSION);

    if (TARGET_GL_MAJOR_VERSION >= 3) NUKLEAR_ENABLE = true;
    else {
        NUKLEAR_ENABLE = false;
        printf("NUKLEAR disabled due to low OpenGL Version");
    }

    // SDL_GL_SetSwapInterval(1); // Use VSYNC

    // // Initialize GL Extension Wrangler (GLEW)
    // glewExperimental = GL_TRUE; // Please expose OpenGL 3.x+ interfaces
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to init GLEW\n");
        SDL_GL_DeleteContext(fs_sdl_GLcontext);
        SDL_DestroyWindow(fs_sdl_window);
        SDL_Quit();
        return 1;
    }
    
    printf("DRIVER GL INFO : Version %s,\n", glGetString(GL_VERSION));
    printf("DRIVER GL INFO : Shader Version %s,\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("DRIVER GL INFO : Vendor %s,\n", glGetString(GL_VENDOR));
    printf("DRIVER GL INFO : Renderer %s\n", glGetString(GL_RENDERER));

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glFrontFace(GL_CCW);
    glDisable(GL_DEPTH_CLAMP);
    printf("Set GL successfully.\n");

    #ifndef NODEBUG
        // During init, enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        if (TARGET_GL_MAJOR_VERSION >= 4 && TARGET_GL_MINOR_VERSION >= 3){
            printf("Enable glDeugMessageCallback\n");
            glDebugMessageCallback(MessageCallback, 0);
        }
    #endif

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
    printf("Set PATH successfully.\n");
    if (PYTHON_ENABLE) {
        Py_Initialize();
        printf("Inited Python successfully.\n");

        PyObject *sys = PyImport_ImportModule("sys");
        PyObject *path = PyObject_GetAttrString(sys, "path");
        PyList_Append(path, PyUnicode_FromString(path_python_addon));
        PyList_Append(path, PyUnicode_FromString(path_python_script));

        if (FS_PyConsole_init()) {
            fprintf(stderr, "Initialize Python Console Failed\n");
        }
    }

    // Load Model
    GLint status;
    char err_buf[4096];

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    printf("Binded default VAO successfully.\n");

    char* tmp_shader_src;
    tmp_shader_src = file_to_mem("default.vert.glsl");
    m_vert_shader = FScreateShader(GL_VERTEX_SHADER, tmp_shader_src);
    free(tmp_shader_src);
    if (m_vert_shader == -1) return 1;

    tmp_shader_src = file_to_mem("default.frag.glsl");
    m_frag_shader = FScreateShader(GL_FRAGMENT_SHADER, tmp_shader_src);
    free(tmp_shader_src);
    if (m_frag_shader == -1) return 1;

    m_shader_prog = glCreateProgram();
    glAttachShader(m_shader_prog, m_vert_shader);
    glAttachShader(m_shader_prog, m_frag_shader);
    glBindFragDataLocation(m_shader_prog, 0, "out_Color0");
    glLinkProgram(m_shader_prog);
    glUseProgram(m_shader_prog);

    int model_result =  loadOBJ("resources/teapot.obj", &teapot_vert_size,  &teapot_vertex_array,
                                                        &teapot_index_size, &teapot_index_array);
    printf("%s loaded OBJ.\n", (model_result ? "Failed to" : "Successfully"));

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

    if (FS_GLscreenInit(current_resolution_x, current_resolution_y)) {
        printf("Failed to initialize core screen!\n");
        exit(1);
    }

    if (NUKLEAR_ENABLE) FS_nk_consoleInit(fs_sdl_window);

    render.node_start = create_node(NULL, NULL, NULL,
                                    create_data(create_dynamic_str("Teapot_parent", 13), 14));
    node* teapot_node_1 = create_node(render.node_start,
                                      NULL, NULL,
                                      create_data(create_dynamic_str("Teapot_1", 8), 9));
    node* teapot_node_2 = create_node(render.node_start,
                                      NULL, NULL,
                                      create_data(create_dynamic_str("Teapot_2", 8), 9));

    return 0;
}

/*
 * Free resources
 */
int FS_clean_up() {
    printf("Exiting...\n");

    if (PYTHON_ENABLE && Py_FinalizeEx() < 0) return 120;

    if (NUKLEAR_ENABLE) nk_sdl_shutdown();

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

    SDL_GL_DeleteContext(fs_sdl_GLcontext);
    SDL_DestroyWindow(fs_sdl_window);
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

    // Render NodeTree
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glUseProgram(m_shader_prog);
    glDrawElements(GL_TRIANGLES, teapot_index_size, GL_UNSIGNED_INT, 0);

    // Render CoreScreen
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

    if (NUKLEAR_ENABLE) nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

    SDL_GL_SwapWindow(fs_sdl_window);
    return 0;
}

void SDL_onResize(int w, int h) {
    if (!w || !h)
        SDL_GetWindowSize(fs_sdl_window, &current_resolution_x, &current_resolution_y);
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


    const float ratio[] = {0.9f, 0.1f};
    char *box_buffer = (char*)malloc(sizeof(char)*1024);
    if (box_buffer == NULL) {
        printf("Error while malloc for console log box\n");
        return 1;
    }
    memset(box_buffer, 0, sizeof(char)*1024);
    int box_buffer_size = 1024;
    int box_len = 0;
    char py_con_input_str[2048] = { 0 };
    int py_con_input_len = 0;
    bool py_con_continue = false;

    strcat(box_buffer, "Python console for FoxSnow Engine\n");
    box_len = 34;

    printf("Running...\n");
    while(should_run) {
        start_tick = SDL_GetTicks();
        SDL_Event event;
        if (NUKLEAR_ENABLE) nk_input_begin(fs_nk_context);
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

                        SDL_SetWindowFullscreen(fs_sdl_window, (mode_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
                        SDL_onResize(0, 0);
                    }
                    else if (event.key.keysym.sym == SDLK_TAB) mode_multiple_viewport = !mode_multiple_viewport;
                    else if (event.key.keysym.sym == SDLK_ESCAPE) should_run = false;
                }
            }
            if (NUKLEAR_ENABLE) nk_sdl_handle_event(&event);
        }
        if (NUKLEAR_ENABLE) nk_input_end(fs_nk_context);

        if (mode_console && PYTHON_ENABLE && NUKLEAR_ENABLE) {
            if (nk_begin(fs_nk_context, "Interactive Console", nk_rect(30, 70, 480, 320),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {

                nk_flags active;
                float window_height = nk_window_get_height(fs_nk_context);
                float window_width  = nk_window_get_width(fs_nk_context);

                nk_layout_row_dynamic(fs_nk_context, window_height - 90, 1);
                nk_edit_string(fs_nk_context, NK_EDIT_BOX|NK_EDIT_GOTO_END_ON_ACTIVATE, box_buffer, &box_len, INT_MAX, nk_filter_default);

                nk_layout_row_begin(fs_nk_context, NK_STATIC, 25, 3);
                nk_layout_row_push(fs_nk_context, 20);
                nk_label(fs_nk_context, (py_con_continue ? "..." : ">>>"), NK_TEXT_LEFT);

                nk_layout_row_push(fs_nk_context, window_width - 125);
                active = nk_edit_string(fs_nk_context, NK_EDIT_FIELD|NK_EDIT_SIG_ENTER|NK_EDIT_ALLOW_TAB, py_con_input_str, &py_con_input_len, 64,  nk_filter_default);

                nk_layout_row_push(fs_nk_context, 69);
                if (nk_button_label(fs_nk_context, "Submit") || (active & NK_EDIT_COMMITED)) {
                    if (py_con_continue || py_con_input_str[0] !=  '\0') {
                        // Write console input to console log box
                        box_buffer_size += (py_con_input_len + 5);
                        box_buffer = (char*)realloc(box_buffer, box_buffer_size);
                        box_len += (py_con_input_len + 5);
                        strcat(box_buffer, (py_con_continue ? "... " : ">>> "));
                        strcat(box_buffer, py_con_input_str);
                        strcat(box_buffer, "\n");

                        // Push input to Console Object(Python)
                        PyObject *result = FS_PyConsole_push(py_con_input_str);

                        if (result == NULL) printf("Error while getting result of console\n");
                        else {
                            // Get boolean whether input is continuous or end
                            py_con_continue = PyObject_IsTrue(PyTuple_GetItem(result, 0));
                            // Get result string and length
                            int py_con_result_len = 0;
                            // Convert Python string to C char array
                            char *py_con_result_str = PyUnicode_AsUTF8AndSize(PyTuple_GetItem(result, 1), &py_con_result_len);
                            if (py_con_result_str == NULL) printf("Error while parsing string from returned tuple from console\n");
                            else {
                                // Copy result string to console log
                                box_buffer_size += py_con_result_len;
                                box_buffer = (char*)realloc(box_buffer, box_buffer_size);
                                box_len += py_con_result_len;
                                strcat(box_buffer, py_con_result_str);

                                // Clear result 
                                Py_XDECREF(result);

                                // Clear result string
                                free(py_con_result_str);
                                py_con_result_len = 0;
                            }
                        memset(py_con_input_str, 0, sizeof(py_con_input_str));
                        py_con_input_len = 0;
                        }
                    }
                }
                nk_layout_row_end(fs_nk_context);

            }
            nk_end(fs_nk_context);
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