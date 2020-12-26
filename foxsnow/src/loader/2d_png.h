#ifndef FS_LIB_LOADER_W3C_PNG
	#define FS_LIB_LOADER_W3C_PNG

	#include <stdio.h>
	#include <stdlib.h>
	#include <ctype.h>
	#include <string.h>
    
    #include <libpng16/png.h>

	#include "../fs_opengl.h"
	#include "../fs_stdfunc.h"
	#include "../utils/fs_debug_print.h"

	#define PNG_BYTES_TO_CHECK 8

	TextureInfo* loadPNG(const char* path, GLint filter_min, GLint filter_mag, GLint wrap_s, GLint wrap_t);
#endif