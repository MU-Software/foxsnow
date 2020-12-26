#ifndef FS_LIB_TYPE_MATRIX
#define FS_LIB_TYPE_MATRIX
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../utils/fs_debug_print.h"

typedef struct _matrix {
	int width;
	int height;
	float* mat;
} matrix;
#define MAT(m,r,c) (m->mat[(r)*m->width+(c)])

matrix* create_matrix(int height, int width, ...);
matrix* create_identity_matrix(int height);
void free_matrix(matrix** mat_pointer);

void mat_print(matrix mat);
matrix* mat_plus(matrix* a, matrix* b);
matrix* mat_minus(matrix* a, matrix* b);
matrix* mat_scala_multiply(matrix* a, float b);
matrix* mat_scala_divide(matrix* a, float b);
matrix* mat_multiply(matrix* a, matrix* b);
matrix* mat_square_multiply(matrix* a, matrix* b);
matrix* mat_divide(matrix* a, matrix* b);
matrix* mat_transpose(matrix* target);

void mat_test(void);
#endif