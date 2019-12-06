// #ifndef FS_NUKLEAR
//     #define FS_NUKLEAR

//     #ifndef GLEW_STATIC
//         #define GLEW_STATIC
//     #endif
//     #ifndef __glew_h__
//         #include <GL/glew.h>
//     #endif

//     #define NK_INCLUDE_FIXED_TYPES
//     #define NK_INCLUDE_STANDARD_IO
//     #define NK_INCLUDE_STANDARD_VARARGS
//     #define NK_INCLUDE_DEFAULT_ALLOCATOR
//     #define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
//     #define NK_INCLUDE_FONT_BAKING
//     #define NK_INCLUDE_DEFAULT_FONT
//     #define NK_IMPLEMENTATION
//     #define NK_SDL_GL3_IMPLEMENTATION
//     #include "nuklear.h"
//     #include "nuklear_sdl_gl3.h"

//     #define MAX_VERTEX_MEMORY 512 * 1024
//     #define MAX_ELEMENT_MEMORY 128 * 1024
    
//     struct nk_context *fs_nk_context;
//     struct nk_font_atlas *fs_nk_font_atlas;

//     void FS_nk_consoleInit(SDL_Window* window);
// #endif