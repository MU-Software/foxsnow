#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <SDL2/SDL.h>
#ifdef _DEBUG
#pragma comment(lib, "SDL2maind.lib")
#else
#pragma comment(lib, "SDL2main.lib")
#endif

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#ifndef __glew_h__
#include <GL/glew.h>
#endif

#include "src/fs_stdfunc.h"
#include "src/utils/fs_debug_print.h"
#include "src/datatype/fs_node.h"
#include "src/datatype/fs_3d_data.h"
#include "src/fs_opengl.h"
#include "src/fs_node_render.h"
#include "src/fs_node_control.h"
#include "src/loader/3d_obj.h"

#define FS_ENABLE_PYTHON_SUPPORT

#ifdef FS_ENABLE_PYTHON_SUPPORT
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#else
typedef void PyObject;
#endif

char* title = NULL;

int TARGET_GL_MAJOR_VERSION = 0;
int TARGET_GL_MINOR_VERSION = 0;

const int FPS_LIMIT = 60;
unsigned int current_resolution_x = 800;
unsigned int current_resolution_y = 600;

unsigned long long frame_number = 0;
unsigned long long start_tick = 0;

double rads = 1.5708;
float cylindricalYLookAtPos = 0;

SDL_Window* fs_sdl_window;
SDL_GLContext  fs_sdl_GLcontext;

bool mode_multiple_viewport = false;
bool mode_wireframe = false;
bool mode_fullscreen = false;

/*
* Basic Initialization
*/
int FS_SDL2_init() {
    dprint("Initializing...\n");

//#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
//    adjustWindowSize((SHORT)160, (SHORT)40);
//#endif

    // Initialize SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        eprint("Failed to initialize SDL video\n");
        return 1;
    }

    // Create main window
    int sdl_window_status = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    fs_sdl_window = SDL_CreateWindow(
        title ? title : "Foxsnow",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        current_resolution_x, current_resolution_y,
        sdl_window_status);

    if (fs_sdl_window == NULL) {
        dprint("Failed to create main window\n");
        SDL_Quit();
        return 1;
    }
    SDL_CaptureMouse(SDL_FALSE);
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_ShowCursor(SDL_ENABLE);

    // Initialize rendering context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    int TARGET_GL_VERSION_INDEX = 0;
    for (TARGET_GL_VERSION_INDEX; TARGET_GL_VERSION_INDEX <= FS_GL_VERSION_LIST_LEN; TARGET_GL_VERSION_INDEX++) {
        TARGET_GL_MAJOR_VERSION = FS_GL_VERSION_LIST[TARGET_GL_VERSION_INDEX].major;
        TARGET_GL_MINOR_VERSION = FS_GL_VERSION_LIST[TARGET_GL_VERSION_INDEX].minor;
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, TARGET_GL_MAJOR_VERSION);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, TARGET_GL_MINOR_VERSION);

        fs_sdl_GLcontext = SDL_GL_CreateContext(fs_sdl_window);
        if (fs_sdl_GLcontext == NULL) continue;
        else break;
    }
    if (fs_sdl_GLcontext == NULL) {
        dprint("Failed to create GL context(TOTAL_FAILURE)\n");
        SDL_DestroyWindow(fs_sdl_window);
        SDL_Quit();
        return 1;
    }
    dprint("Created GL context successfully on OpenGL %d.%d.\n", TARGET_GL_MAJOR_VERSION, TARGET_GL_MINOR_VERSION);

    // Use VSYNC
    if (SDL_GL_SetSwapInterval(-1)) // Try adaptive vsync
        if (SDL_GL_SetSwapInterval(1)) // Failover to adaptive vsync, try certical retrace
            SDL_GL_SetSwapInterval(0); // Failover to certical retrace, Just turn off the vsync
    dprint("Grahpics Vsync support level : %d\n", SDL_GL_GetSwapInterval());
    dprint("(-1 is best, 1 is normal, 0 is not good.)\n");

    // // Initialize GL Extension Wrangler (GLEW)
     glewExperimental = GL_TRUE; // Please expose OpenGL 3.x+ interfaces
    if (glewInit() != GLEW_OK) {
        dprint("Failed to init GLEW\n");
        SDL_GL_DeleteContext(fs_sdl_GLcontext);
        SDL_DestroyWindow(fs_sdl_window);
        SDL_Quit();
        return 1;
    }

    dprint("DRIVER GL INFO : Version %s\n", glGetString(GL_VERSION));
    dprint("DRIVER GL INFO : Shader Version %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    dprint("DRIVER GL INFO : Vendor %s\n", glGetString(GL_VENDOR));
    dprint("DRIVER GL INFO : Renderer %s\n", glGetString(GL_RENDERER));

    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glFrontFace(GL_CW);
    glDepthFunc(GL_LESS);
    // glDisable(GL_DEPTH_CLAMP);
    glLineWidth(1.5f);

    GLenum GL_ERROR = 0;
    if ((GL_ERROR = glGetError()) != GL_NO_ERROR) {
        dprint("Failed to set some GL state.(GL Error: %d)\n", GL_ERROR);
        SDL_GL_DeleteContext(fs_sdl_GLcontext);
        SDL_DestroyWindow(fs_sdl_window);
        SDL_Quit();
        return 1;
    }
    dprint("GL initialization success.\n");

#ifdef _DEBUG
    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    if (TARGET_GL_MAJOR_VERSION >= 4 && TARGET_GL_MINOR_VERSION >= 3) {
        dprint("Enable glDeugMessageCallback\n");
        glDebugMessageCallback(MessageCallback, 0);
    }
#endif

    FS_ViewMatrix = create_identity_matrix(4, 4);
    FS_ProjectionMatrix = create_matrix(4, 4,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f);
    FScalculatePerspectiveMatrix(FS_ProjectionMatrix, 40.f, 800.f/600.f, 1.f, 100.f);

    camera.lookPoint[2] = -1.0f;
    commitCamera();

    if (FS_GLscreenInit(current_resolution_x, current_resolution_y))
        eprint("Failed to initialize core screen!\n");

    return 0;
}

/*
 * Free resources
 */
int FS_clean_up() {
    dprint("Exiting...\n");

    glUseProgram(0);
    glDisableVertexAttribArray(0);

    SDL_GL_DeleteContext(fs_sdl_GLcontext);
    SDL_DestroyWindow(fs_sdl_window);
    SDL_Quit();

    return 0;
}

/*
 * Render a frame
 */
int OGL_render_update() {
    GLenum err = GL_NO_ERROR;
    glPolygonMode(GL_FRONT_AND_BACK, (mode_wireframe ? GL_LINE : GL_FILL));
    glBindFramebuffer(GL_FRAMEBUFFER, FS_CoreFrameBuffer);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render NodeTree
    renderNode((render.node_start), 0);

    // Render CoreScreen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(FS_CoreScreenQuadVAO);
    glUseProgram(FS_CoreScreenShaderProgram);
    GLint uniform_frame_number_loc = glGetUniformLocation(FS_CoreScreenShaderProgram, "fs_FrameNumber");
    GLint uniform_screen_size_loc = glGetUniformLocation(FS_CoreScreenShaderProgram, "fs_ScreenSize");
    glUniform1ui(uniform_frame_number_loc, frame_number);
    glUniform2ui(uniform_screen_size_loc, current_resolution_x, current_resolution_y);

    if (mode_multiple_viewport) {
        int multi_scrn_tgt_x = current_resolution_x / 3;
        int multi_scrn_tgt_y = current_resolution_y / 3;

        // Main
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[0].texture_id);
        glViewport(0, multi_scrn_tgt_y + 1, multi_scrn_tgt_x * 2 + 1, multi_scrn_tgt_y * 2);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 1X3
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[0].texture_id);
        glViewport(multi_scrn_tgt_x * 2 + 2, multi_scrn_tgt_y * 2 + 2, multi_scrn_tgt_x, multi_scrn_tgt_y);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 2X3
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[1].texture_id);
        glViewport(multi_scrn_tgt_x * 2 + 2, multi_scrn_tgt_y + 1, multi_scrn_tgt_x, multi_scrn_tgt_y);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 3X3
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[2].texture_id);
        glViewport(multi_scrn_tgt_x * 2 + 2, 0, multi_scrn_tgt_x, multi_scrn_tgt_y);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 3X1
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[3].texture_id);
        glViewport(0, 0, multi_scrn_tgt_x, multi_scrn_tgt_y);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 3X2
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[0].texture_id);
        glViewport(multi_scrn_tgt_x + 1, 0, multi_scrn_tgt_x, multi_scrn_tgt_y);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    else {
        // Main only
        glBindTexture(GL_TEXTURE_2D, FS_CoreFrameBufferTexture[0].texture_id);
        glViewport(0, 0, current_resolution_x, current_resolution_y);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    SDL_GL_SwapWindow(fs_sdl_window);
    return 0;
}

void SDL_onResize() {
    SDL_GetWindowSize(fs_sdl_window, &current_resolution_x, &current_resolution_y);

    FScalculatePerspectiveMatrix(FS_ProjectionMatrix, 40.f, (float)current_resolution_x / (float)current_resolution_y, 1.f, 100.f);
    commitCamera();

    glDeleteFramebuffers(1, &FS_CoreFrameBuffer);
    for (int z = 0; z < 4; z++) {
        glDeleteTextures(1, &FS_CoreFrameBufferTexture[z].texture_id);
    }

    glGenFramebuffers(1, &FS_CoreFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, FS_CoreFrameBuffer);

    /* FBO - Generate Texture */
    for (int z = 0; z < 4; z++)
        FSgenerateTextureFBO(
            FS_CoreFrameBuffer,
            current_resolution_x,
            current_resolution_y,
            &FS_CoreFrameBufferTexture[z]);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        dprint("FS GL : FRAMEBUFFER NOT COMPLETE");
        exit(1);
    }

    dprint("                                           \r");
    dprint("FS SDL : EVENT RESIZE %d %d\n", current_resolution_x, current_resolution_y);
}

void test_env_setup() {
    // Load Model
    float scale = 0.01f;
    render.node_start = create_node(NULL, renderNodeIn, renderNodeOut, create_data(), strdup("Teapot_parent"));
    node* teapot_node_1 = create_node(render.node_start,
        renderNodeIn, renderNodeOut,
        create_data(), strdup("Teapot_1"));
    FSnode_setShader(teapot_node_1, "default");
    FSnode_loadOBJ(teapot_node_1, "skysphere.obj");

    node* teapot_node_2 = create_node(render.node_start,
        renderNodeIn, renderNodeOut,
        create_data(), strdup("Teapot_2"));
    FSnode_setShader(teapot_node_2, "default");
    FSnode_loadOBJ(teapot_node_2, "capsule.obj");

    set_scale((fs_3d_data*)(teapot_node_2->data), scale, scale, scale);

    node* teapot_node_3 = create_node(teapot_node_1,
        renderNodeIn, renderNodeOut,
        create_data(), strdup("Teapot_3"));
    FSnode_setShader(teapot_node_3, "default");
    FSnode_loadOBJ(teapot_node_3, "teapot.obj");

    set_scale((fs_3d_data*)(teapot_node_1->data), 0.5f, 0.5f, 0.5f);
    set_scale((fs_3d_data*)(teapot_node_3->data), scale, scale, scale);

    set_z((fs_3d_data*)(teapot_node_1->data), -0.35f);
    set_z((fs_3d_data*)(teapot_node_3->data), -0.1f);
    set_x((fs_3d_data*)(teapot_node_3->data), -0.1f);

}

int FS_runmain(PyObject* fs_py_gamemgr) {
    bool should_run = true;
    double fps = 0.0;

    unsigned long long last_render_time = get_elapsed_time();
    double target_frame_time = (1.0 / (double)FPS_LIMIT) * 1000000;
    double margin_frame_time = target_frame_time / 4;

    // If we can't initialize engine properly,
    // then it's better to terminate whole program than left something.
    //FS_SDL2_init();
    //test_env_setup();

#ifdef FS_ENABLE_PYTHON_SUPPORT
#ifdef Py_DEBUG
    _Py_HashSecret_Initialized = 1;
#endif
    PyObject* fs_py_gamemgr_start_mtd = NULL;
    PyObject* fs_py_gamemgr_update_mtd = NULL;
    //Py_Initialize();
    //PyEval_InitThreads();
    if (fs_py_gamemgr) {
        dprint("Python Game Manager Object received. ID = %p\n", fs_py_gamemgr);

        // Find methods, if one of them are not available, then just shutdown all python extensions
        fs_py_gamemgr_start_mtd = PyObject_GetAttrString(fs_py_gamemgr, "on_start");
        fs_py_gamemgr_update_mtd = PyObject_GetAttrString(fs_py_gamemgr, "on_update");

        if (fs_py_gamemgr_start_mtd && PyCallable_Check(fs_py_gamemgr_start_mtd)
            && fs_py_gamemgr_update_mtd && PyCallable_Check(fs_py_gamemgr_update_mtd)) {

            if (PyErr_Occurred()) {
                PyErr_Print();
                dprint("Exception raised while getting methods from GameManager.\n");

                Py_XDECREF(fs_py_gamemgr_start_mtd);
                Py_XDECREF(fs_py_gamemgr_update_mtd);
                fs_py_gamemgr = NULL;
                fs_py_gamemgr_start_mtd = NULL;
                fs_py_gamemgr_update_mtd = NULL;
            }
        }
        else {
            dprint("Could not found some methods from GameManager.\n");

            Py_XDECREF(fs_py_gamemgr_start_mtd);
            Py_XDECREF(fs_py_gamemgr_update_mtd);
            fs_py_gamemgr = NULL;
            fs_py_gamemgr_start_mtd = NULL;
            fs_py_gamemgr_update_mtd = NULL;
        }
    }
#endif


#ifdef FS_ENABLE_PYTHON_SUPPORT
    if (fs_py_gamemgr) {
        PyObject_CallObject(fs_py_gamemgr_start_mtd, NULL);
        Py_XDECREF(fs_py_gamemgr_start_mtd);
        fs_py_gamemgr_start_mtd = NULL;

        if (PyErr_Occurred()) {
            PyErr_Print();
            dprint("Exception raised while calling GameManager.on_start method.\n");
            Py_XDECREF(fs_py_gamemgr_update_mtd);
            fs_py_gamemgr = NULL;
            fs_py_gamemgr_update_mtd = NULL;
        }
        test_env_setup();
    }
    else {
#endif
        // This code will be executed only when Python support is disabled or py_gamemgr is NULL.
        // This means that test scene will be set when Python is unavailable.
        test_env_setup();
#ifdef FS_ENABLE_PYTHON_SUPPORT
    }
#endif

    bool mouse_right_pressed = false;
    bool mouse_left_pressed = false;
    bool mouse_middle_pressed = false;

    dprint("Running...\n");
    while (should_run) {
        start_tick = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                should_run = false;
                break;
            }
            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_onResize();
            }
            else if (event.type == SDL_KEYDOWN) {
                float cammove = 0.001;
                if (event.key.keysym.sym == SDLK_ESCAPE) should_run = false;

                else if (event.key.keysym.sym == SDLK_w) {
                    camera.pos[2] += cammove;
                    commitCamera();
                }
                else if (event.key.keysym.sym == SDLK_a) {
                    camera.pos[0] += cammove;
                    commitCamera();
                }
                else if (event.key.keysym.sym == SDLK_s) {
                    camera.pos[2] -= cammove;
                    commitCamera();
                }
                else if (event.key.keysym.sym == SDLK_d) {
                    camera.pos[0] -= cammove;
                    commitCamera();
                }
                else if (event.key.keysym.sym == SDLK_q) {
                    camera.pos[1] -= cammove;
                    commitCamera();
                }
                else if (event.key.keysym.sym == SDLK_e) {
                    camera.pos[1] += cammove;
                    commitCamera();
                }

// Below shortcuts will be available only on Debug build.
#ifdef _DEBUG
                // Framerate change
                else if (event.key.keysym.sym == SDLK_F1) target_frame_time = (1.0 / 1.0) * 1000000;
                else if (event.key.keysym.sym == SDLK_F2) target_frame_time = (1.0 / 10.0) * 1000000;
                else if (event.key.keysym.sym == SDLK_F3) target_frame_time = (1.0 / 30.0) * 1000000;
                else if (event.key.keysym.sym == SDLK_F4) target_frame_time = (1.0 / 60.0) * 1000000;

                // Shader reload for shader debug purpose
                else if (event.key.keysym.sym == SDLK_F7) {
                    node* target_node = render.node_start->child_last;
                    char* target_name = strdup(((fs_3d_data*)target_node->data)->shader_name);
                    dprint("Reloaded %s shader!\n", target_name);
                    FSnode_unsetShader(target_node);
                    FSnode_setShader(target_node, target_name);
                    free(target_name);
                }

                // Camera re-centering
                //else if (event.key.keysym.sym == SDLK_F9) {
                //    spherical_coord_theta = 90.0f;
                //    spherical_coord_phi = 90.0f;
                //    double spherical_coord_theta_rad = getRadian(spherical_coord_theta);
                //    double spherical_coord_phi_rad = getRadian(spherical_coord_phi);
                //    camera.lookPoint[0] = sin(spherical_coord_theta_rad) * cos(spherical_coord_phi_rad) + camera.pos[0];
                //    camera.lookPoint[1] = cos(spherical_coord_theta_rad) * sin(spherical_coord_phi_rad) + camera.pos[1];
                //    camera.lookPoint[2] = sin(spherical_coord_theta_rad) + camera.pos[2];
                //    commitCamera();
                //}

                // Framebuffer debugging
                else if (event.key.keysym.sym == SDLK_F10) mode_multiple_viewport = !mode_multiple_viewport;

                // Full Screen enabler
                else if (event.key.keysym.sym == SDLK_F11) {
                    // SDL_WINDOW_FULLSCREEN will do 'real' fullscreen with video mode change,
                    // while SDL_WINDOW_FULLSCREEN_DESKTOP will do "fake" fullscreen
                    // that takes the size of the desktop.
                    mode_fullscreen = !mode_fullscreen;
                    SDL_SetWindowFullscreen(fs_sdl_window, (mode_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
                    SDL_onResize();
                }

                // Wireframe debugging
                else if (event.key.keysym.sym == SDLK_F12) mode_wireframe = !mode_wireframe;
#endif
            }
            else if (event.type == SDL_KEYUP) {}

            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                bool* target_btn = NULL;
                if (event.button.button == SDL_BUTTON_LEFT) target_btn = &mouse_left_pressed;
                else if (event.button.button == SDL_BUTTON_RIGHT) target_btn = &mouse_right_pressed;
                else target_btn = &mouse_middle_pressed;

                if (!(*target_btn)) (*target_btn) = true;
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {
                bool* target_btn = NULL;
                if (event.button.button == SDL_BUTTON_LEFT) target_btn = &mouse_left_pressed;
                else if (event.button.button == SDL_BUTTON_RIGHT) target_btn = &mouse_right_pressed;
                else target_btn = &mouse_middle_pressed;

                (*target_btn) = false;

                SDL_CaptureMouse(SDL_FALSE);
                SDL_SetRelativeMouseMode(SDL_FALSE);
                SDL_ShowCursor(SDL_ENABLE);
            }
            else if (event.type == SDL_MOUSEMOTION) {
                if (mouse_left_pressed) {
                    SDL_CaptureMouse(SDL_TRUE);
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                    SDL_ShowCursor(SDL_DISABLE);

                    rads -= event.motion.xrel * 0.005f;

                    float old_cylindricalYLookAtPos = cylindricalYLookAtPos;
                    cylindricalYLookAtPos -= event.motion.yrel * 0.02f;
                    if (-15.0f > cylindricalYLookAtPos || cylindricalYLookAtPos > 25.0f)
                        cylindricalYLookAtPos = old_cylindricalYLookAtPos;

                    camera.lookPoint[0] = camera.pos[0] + (10.0f * cos(rads));
                    camera.lookPoint[1] = camera.pos[1] + cylindricalYLookAtPos;
                    camera.lookPoint[2] = camera.pos[2] - (10.0f * sin(rads));
                    commitCamera();

                    SDL_WarpMouseInWindow(fs_sdl_window, current_resolution_x / 2, current_resolution_y / 2);
                }
            }
        }

        // Render frame with proper FPS timing
        double tmp_last_render_time = 0;
        if ((tmp_last_render_time = (double)get_elapsed_time()) > (double)last_render_time + target_frame_time - margin_frame_time) {

#ifdef FS_ENABLE_PYTHON_SUPPORT
            if (fs_py_gamemgr) {
                PyObject_CallObject(fs_py_gamemgr_update_mtd, NULL);

                if (PyErr_Occurred()) {
                    PyErr_Print();
                    dprint("Exception raised while calling GameManager.on_start method.\n");
                    Py_XDECREF(fs_py_gamemgr_update_mtd);
                    fs_py_gamemgr = NULL;
                    fs_py_gamemgr_update_mtd = NULL;
                }
            }
            else {
#endif
                // This code will be executed only when Python support is disabled or py_gamemgr is NULL
                set_p((fs_3d_data*)(render.node_start->child->data), (float)(frame_number) / 24000.0f * 360.0f);
                set_z((fs_3d_data*)(render.node_start->child_last->data), -((frame_number % 100) * 0.0075f));
#ifdef FS_ENABLE_PYTHON_SUPPORT
            }
#endif

            if (OGL_render_update()) eprint("Exception raised while render!\n");

            frame_number++;
            fps = 1000.0 / (float)(SDL_GetTicks() - start_tick);
            double frame_time = tmp_last_render_time - last_render_time;
            //dprint("FPS : %15lf(%lfms), Frame : %llu\r", fps, frame_time / 1000, frame_number);
            last_render_time = tmp_last_render_time;
        }
    }

#ifdef FS_ENABLE_PYTHON_SUPPORT
    if (fs_py_gamemgr) {
        Py_XDECREF(fs_py_gamemgr_update_mtd);
        fs_py_gamemgr_update_mtd = NULL;
    }
#endif

    dprint("\n");
    return FS_clean_up();
}

int main(int argc, char* argv[]) {
    FS_SDL2_init();
    FS_runmain(NULL);
    return 0;
}