#define GLEW_STATIC
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <Python/Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "fs_py_loader_obj.h"
#include "logo.h"

#ifdef _WIN32
    #include "windows.h"
#endif

#define True 1
#define False 0

const int FPS_LIMIT = 60;

SDL_Window    *m_window;
SDL_GLContext  m_context;
GLuint         m_vao, m_vbo, m_ebo, m_tex;
GLuint         m_vert_shader;
GLuint         m_frag_shader;
GLuint         m_shader_prog;

int Initialize();
int OGL_render_update();
int FS_clean_up();
unsigned int HashCode();
char* file_to_mem(char* path);
GLuint FScreateShader(GLenum shader_type, char* src);
GLuint FScreateTexture(int size, unsigned char* data);

unsigned int HashCode(const char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = 65599 * hash + str[0];
        str++;
    }
    return hash ^ (hash >> 16);
}

char* file_to_mem(char* path) {
	char *str_storage = NULL;
	FILE *fp = fopen(path, "r");
	if (fp != NULL) {
		if (fseek(fp, 0L, SEEK_END) == 0) {
			/* Get the size of the file. */
			long file_size = ftell(fp);
			if (file_size == -1) {
				printf("Error while calculating file size! C");
				fclose(fp);
				return 0;
			} else if (!file_size) {
				printf("File is empty!");
				fclose(fp);
				return 0;
			}

			str_storage = malloc(sizeof(char) * (file_size + 1));

			if (fseek(fp, 0L, SEEK_SET)) {
				printf("Error while calculating file size! B");
				fclose(fp);
				return 0;
			}

			/* Read the entire file into memory. */
			size_t newLen = fread(str_storage, sizeof(char), file_size, fp);
			if (ferror(fp) != 0) {
				fputs("Error while reading file", stderr);
			} else { str_storage[newLen++] = '\0'; /* Just to be safe. */ }
		} else {
				printf("Error while calculating file size! A");
				fclose(fp);
				return 0;
		}
		fclose(fp);
	} else { printf("Cannot open file!"); return 0; }
	return str_storage;
}

char* get_exe_path(char* path, int size) {

    #ifdef _WIN32
    GetModuleFileName(NULL, path, size);
    #elif __linux__
    readlink("/proc/self/exe", path, size);
    #elif  __APPLE__ 
    readlink("/proc/curproc/file", path, size);
    #elif __FreeBSD__
    readlink("/proc/curproc/file", path, size);
    #endif

    return path;
}

char* str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

char* path[65535] = { 0 };
char* path_python[65535] = { 0 };
char* path_python_addon[65535] = { 0 };
char* path_python_script[65535] = { 0 };

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

const GLfloat verts[6][4] = {
    //  x      y      s      t
    { -1.0f, -1.0f,  0.0f,  1.0f }, // BL
    { -1.0f,  1.0f,  0.0f,  0.0f }, // TL
    {  1.0f,  1.0f,  1.0f,  0.0f }, // TR
    {  1.0f, -1.0f,  1.0f,  1.0f }, // BR
};
const GLint indicies[] = { 0, 1, 2, 0, 2, 3 };

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
    m_window = SDL_CreateWindow(
        "FoxSnow",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480, SDL_WINDOW_OPENGL);

    if (m_window == NULL) {
        fprintf(stderr, "Failed to create main window\n");
        SDL_Quit();
        return 1;
    }

    // Initialize rendering context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
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

    // Initialize GL Extension Wrangler (GLEW)
    glewExperimental = GL_TRUE; // Please expose OpenGL 3.x+ interfaces
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to init GLEW\n");
        SDL_GL_DeleteContext(m_context);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 1;
    }

    /* Initialize Shaders */
    GLint status;
    char err_buf[4096];

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    char* vert_shader_src = file_to_mem("core_screen.vert.glsl");
    m_vert_shader = FScreateShader(GL_VERTEX_SHADER, vert_shader_src);
    free(vert_shader_src);
    if (m_vert_shader == -1) return 1;

    char* frag_shader_src = file_to_mem("core_screen.frag.glsl");
    m_frag_shader = FScreateShader(GL_FRAGMENT_SHADER, frag_shader_src);
    free(frag_shader_src);
    if (m_frag_shader == -1) return 1;

    m_shader_prog = glCreateProgram();
    glAttachShader(m_shader_prog, m_vert_shader);
    glAttachShader(m_shader_prog, m_frag_shader);
    glBindFragDataLocation(m_shader_prog, 0, "out_Color0");
    glLinkProgram(m_shader_prog);
    glUseProgram(m_shader_prog);

    /* Initialize Geometry */
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Populate element buffer
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    // Bind vertex position attribute
    GLint pos_attr_loc = glGetAttribLocation(m_shader_prog, "fs_Vertex");
    glVertexAttribPointer(pos_attr_loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(pos_attr_loc);

    // Bind vertex texture coordinate attribute
    GLint tex_attr_loc = glGetAttribLocation(m_shader_prog, "fs_MultiTexCoord0");
    glVertexAttribPointer(tex_attr_loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
    glEnableVertexAttribArray(tex_attr_loc);

    /* Initialize textures */
    GLuint screen_tex = FScreateTexture(256, logo_rgba);
    glUniform1i(glGetUniformLocation(m_shader_prog, "tex_1"), 0);

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

    // printf("%s\n", path);
    // printf("%s\n", path_python);
    // printf("%s\n", path_python_addon);

    SDL_setenv("PYTHONHOME", path_python, true);
    SDL_setenv("PYTHONPATH", path_python, true);
    Py_Initialize();

    PyObject *sys = PyImport_ImportModule("sys");
    PyObject *path = PyObject_GetAttrString(sys, "path");
    PyList_Append(path, PyUnicode_FromString(path_python_addon));
    PyList_Append(path, PyUnicode_FromString(path_python_script));

    PyRun_SimpleString("import time;import numpy;print(numpy.version.version)");

    int teapot_vert_size, teapot_index_size;
    float* teapot_vertex_array;
    int* teapot_index_array;
    FSloadOBJ("resources/teapot.obj", &teapot_vert_size,  teapot_vertex_array,
                                      &teapot_index_size, teapot_index_array);
    printf("result = %d\n", teapot_index_array[100]);
    return 0;
}

/*
 * Free resources
 */
int FS_clean_up() {
    printf("Exiting...\n");

    if (Py_FinalizeEx() < 0) return 120;

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
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    SDL_GL_SwapWindow(m_window);
    return 0;
}

/*
 * Program entry point
 */
int main(int argc, char *argv[]) {
    bool should_run = true, cap = true;
    double fps = 0.0f;
    unsigned long long frame = 0, start_tick = 0;

    if (Initialize()) return 1;

    printf("Running...\n");
    while(should_run) {
        start_tick = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                should_run = false;
                break;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_F12) cap = !cap;
                else if (event.key.keysym.sym == SDLK_ESCAPE) should_run = false;
            }
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