#ifndef FS_LIB_STDFUNC
#define FS_LIB_STDFUNC

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "utils/fs_debug_print.h"

// This is the reason why foxsnow is only for Windows
#include <Windows.h>
struct SMALL_RECT {
	SHORT Left;
	SHORT Top;
	SHORT Right;
	SHORT Bottom;
};
void adjustWindowSize(SHORT x, SHORT y);
unsigned long long get_current_time();
unsigned long long get_elapsed_time();

char* fs_file_to_mem(const char* path);
unsigned long hash(unsigned const char* str);
double getRadian(double degree);
void normalizeVector(float* vec);

#endif