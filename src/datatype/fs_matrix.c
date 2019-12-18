#include "fs_matrix.h"

matrix* create_matrix(int height, int width, ...) {
	va_list ap;
	matrix* mat_return = (matrix*)calloc(1, sizeof(matrix));
	mat_return->height = height;
	mat_return->width = width;
	mat_return->mat = (float*)calloc(height*width, sizeof(float));
	va_start(ap, width);
	/* 	int r, c;
	for (r = 0; r < height; r++) {
		for (c = 0; c < width; c++) {
			float target_val = va_arg(ap, float);
			int target_index = r * width + c;
			mat_return->mat[target_index] = target_val;
		}
	} */
	int index;
	for (index=0; index < height*width; index++)
		mat_return->mat[index] = (float)va_arg(ap, double);
	va_end(ap);
	return mat_return;
}
matrix* create_identity_matrix(int height) {
	matrix* mat_return = (matrix*)calloc(1, sizeof(matrix));
	mat_return->height = height;
	mat_return->width  = height;
	mat_return->mat = (float*)calloc(height*height, sizeof(float));

	int r, c;
	for (r=0; r < height; r++)
		for (c=0; c < height; c++)
			mat_return->mat[r * height + c] = (r == c ? 1 : 0);
	return mat_return;
}
void free_matrix(matrix** mat_pointer) {
	if (mat_pointer) {
		free((*mat_pointer)->mat);
		free(*mat_pointer);
		*mat_pointer = NULL;
	}
	else {
		printf("NULL_PTR on free_matrix = %p\n", mat_pointer);
	}
}

void mat_print(matrix mat) {
	if (!&mat) {
		printf("EMPTY MATRIX RECEIVED!\n");
		return;
	}
	int r, c;
	for (r = 0; r < mat.height; r++) {
		for (c = 0; c < mat.width; c++) {
			printf("%.3f ", mat.mat[r * mat.width + c]);
		}
		printf("\n");
	}
}
matrix* mat_plus(matrix* a, matrix* b) {
	if (!a||!b) {
		printf("MATRIX %c is NULL!\n", (a?'b':'a'));
		return NULL;
	}
	if ((a->width != b->width) || (a->height != b->height)) {
		printf("IMPROPER MATRIX CALCULATION! - PLUS\n");
		return NULL;
	}
	matrix* result = (matrix*)calloc(1, sizeof(matrix));
	result->height = a->height;
	result->width  = a->width;
	result->mat = (float*)calloc(a->height*a->width, sizeof(float));
	memcpy(result->mat, a->mat, a->height*a->width*sizeof(float));

	/* int r, c;
	for (r = 0; r < a->width; r++)
		for (c = 0; c < a->height; c++)
			result->mat[r * a->width + c] += b->mat[r * b->width + c]; */
	
	int index;
	for (index = 0; index < result->height * result->width; index++)
		result->mat[index] += b->mat[index];

	return result;
}
matrix* mat_minus(matrix* a, matrix* b) {
	if (!a||!b) {
		printf("MATRIX %c is NULL!\n", (a?'b':'a'));
		return NULL;
	}
	if ((a->width != b->width) || (a->height != b->height)) {
		printf("IMPROPER MATRIX CALCULATION! - MINUS\n");
		return NULL;
	}
	matrix* result = (matrix*)calloc(1, sizeof(matrix));
	result->height = a->height;
	result->width  = a->width;
	result->mat = (float*)calloc(a->height*a->width, sizeof(float));
	memcpy(result->mat, a->mat, a->height*a->width*sizeof(float));

	/* 	int r, c;
	for (r = 0; r < a->width; r++)
		for (c = 0; c < a->height; c++)
			result->mat[r * a->width + c] -= b->mat[r * b->width + c]; */

	int index;
	for (index = 0; index < result->height * result->width; index++)
		result->mat[index] -= b->mat[index];

	return result;
}
matrix* mat_scala_multiply(matrix* a, float b) {
	if (!a) {
		printf("MATRIX a is NULL!\n");
		return NULL;
	}
	matrix* result = (matrix*)calloc(1, sizeof(matrix));
	result->height = a->height;
	result->width  = a->width;
	result->mat = (float*)calloc(a->height*a->width, sizeof(float));
	memcpy(result->mat, a->mat, a->height*a->width*sizeof(float));

	int index;
	for (index = 0; index < result->height * result->width; index++)
		result->mat[index] *= b;
	return result;
}
matrix* mat_scala_divide(matrix* a, float b) {
	if (!a) {
		printf("MATRIX a is NULL!\n");
		return NULL;
	}
	matrix* result = (matrix*)calloc(1, sizeof(matrix));
	result->height = a->height;
	result->width  = a->width;
	result->mat = (float*)calloc(a->height*a->width, sizeof(float));
	memcpy(result->mat, a->mat, a->height*a->width*sizeof(float));

	int index;
	for (index = 0; index < result->height * result->width; index++)
		result->mat[index] /= b;
	return result;
}
matrix* mat_multiply(matrix* a, matrix* b) {
	if (!a||!b) {
		printf("MATRIX %c is NULL!\n", (a?'b':'a'));
		return NULL;
	}
	if (a->width != b->height) {
		printf("IMPROPER MATRIX CALCULATION! - MULTIPLY\n");
		return NULL;
	}

	matrix* result = (matrix*)calloc(1, sizeof(matrix));
	result->height = a->height;
	result->width  = b->width;
	result->mat = (float*)calloc(result->height*result->width, sizeof(float));
	int r, c;
	for (r = 0; r < result->height; r++)
		for (c = 0; c < result->width; c++) {
			float target_val = 0;
			int index;
			for (index = 0; index < a->width; index++)
				target_val += a->mat[r * a->width + index] * b->mat[index * b->width + c];
			result->mat[r * b->width + c] = target_val;
		}	
	return result;
}
matrix* mat_square_multiply(matrix* a, matrix* b) {
	if (!a||!b) {
		printf("MATRIX %c is NULL!\n", (a?'b':'a'));
		return NULL;
	}
	if (a->width != a-> height || a->height != b->width || b->width != b-> height) {
		printf("WARNING : IMPROPER MATRIX CALCULATION! - SQUARE MATRIX MULTIPLY\n");
		return mat_multiply(a, b);
	}
	int r, c;
	matrix* result = (matrix*)calloc(1, sizeof(matrix));
	result->height = a->height;
	result->width  = b->width;
	result->mat = (float*)calloc(result->height*result->width, sizeof(float));
	for (r = 0; r < result->height; r++)
		for (c = 0; c < result->width; c++) {
			float target_val = 0;
			int index;
			for (index = 0; index < a->width; index++)
				target_val += a->mat[r * a->width + index] * b->mat[index * a->width + c];
			result->mat[r * a->width + c] = target_val;
		}
	return result;
}
matrix* mat_divide(matrix* a, matrix* b) {
	if (!a||!b) {
		printf("MATRIX %c is NULL!\n", (a?'b':'a'));
		return NULL;
	}
	// TODO : Let's make this function work!
}

void mat_test(void) {
	printf("-----Matrix Creation & Free Test--------\n");
	matrix* new_mat = create_matrix(3, 3,
							1.0f, 2.0f, 3.0f,
							4.0f, 5.0f, 6.0f,
							7.0f, 8.0f, 9.0f);
	mat_print(*new_mat);
	printf("-----Matrix Plus & Minus Test-----------\n");
	matrix* mat_res_plus = mat_plus(new_mat, new_mat);
	matrix* mat_res_minus = mat_minus(new_mat, new_mat);
	mat_print(*mat_res_plus);
	/*
	result must be...
	 2  4  6
	 8 10 12
	14 16 18
	*/
	mat_print(*mat_res_minus);
	/*
	result must be...
	 0  0  0
	 0  0  0
	 0  0  0
	*/
	free_matrix(&mat_res_plus);
	free_matrix(&mat_res_minus);
	free_matrix(&new_mat);
	printf("-----Square Matrix Multiply Test--------\n");
	matrix* tmp_mat_1 = create_matrix(4, 4,
							 1.0f,  2.0f,  3.0f,  4.0f,
							 5.0f,  6.0f,  7.0f,  8.0f,
							 9.0f, 10.0f, 11.0f, 12.0f,
							13.0f, 14.0f, 15.0f, 16.0f);
	matrix* tmp_mat_2 = create_matrix(4, 4,
							 1.0f,  2.0f,  3.0f,  4.0f,
							 5.0f,  6.0f,  7.0f,  8.0f,
							 9.0f, 10.0f, 11.0f, 12.0f,
							13.0f, 14.0f, 15.0f, 16.0f);
	matrix* mat_res_square_mul = mat_square_multiply(tmp_mat_1, tmp_mat_2);
	mat_print(*mat_res_square_mul);
	/*
	result must be...
	 90 100 110 120
	202 228 254 280
	314 356 398 440
	426 484 542 600
	*/

	free_matrix(&mat_res_square_mul);
	free_matrix(&tmp_mat_1);
	free_matrix(&tmp_mat_2);
	printf("-----Normal Matrix Multiply Test--------\n");
	matrix* tmp_mat_a = create_matrix(4, 2,
							1.0f,  2.0f, 
							3.0f,  4.0f,
							5.0f,  6.0f,
							7.0f,  8.0f);
	matrix* tmp_mat_b = create_matrix(2, 5,
							1.0f,  2.0f,  3.0f,  4.0f,  5.0f,
							6.0f,  7.0f,  8.0f,  9.0f, 10.0f);
	matrix* mat_res_mul = mat_multiply(tmp_mat_a, tmp_mat_b);
	mat_print(*mat_res_mul);
	/*
	result must be...
	 13  16  19  22  25
	 27  34  41  48  55
	 41  52  63  74  85
	 55  70  85 100 115
	*/
	free_matrix(&mat_res_mul);
	free_matrix(&tmp_mat_a);
	free_matrix(&tmp_mat_b);
}

// int main(void) {
// 	printf("HELLO WORLD!\n");
// 	mat_test();
// }