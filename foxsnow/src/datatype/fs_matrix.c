#include "fs_matrix.h"

matrix* create_matrix(int height, int width, ...) {
	va_list ap;
	matrix* mat_return = (matrix*)calloc(1, sizeof(matrix));
	if (mat_return == NULL) {
		dprint("CALLOC FAILED at create_matrix - matrix!\n");
		exit(1);
	}
	mat_return->height = height;
	mat_return->width = width;
	mat_return->mat = (float*)calloc(height * width, sizeof(float));
	if (mat_return->mat == NULL) {
		dprint("CALLOC FAILED at create_matrix - float!\n");
		exit(1);
	}
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
	for (index = 0; index < height * width; index++)
		mat_return->mat[index] = (float)va_arg(ap, double);
	va_end(ap);
	return mat_return;
}
matrix* create_identity_matrix(int height) {
	matrix* mat_return = (matrix*)calloc(1, sizeof(matrix));
	if (mat_return == NULL) {
		dprint("CALLOC FAILED at create_identity_matrix!\n");
		exit(1);
	}
	mat_return->height = height;
	mat_return->width = height;
	mat_return->mat = (float*)calloc(height * height, sizeof(float));
	if (mat_return->mat == NULL) {
		dprint("CALLOC FAILED at create_identify_matrix - float!\n");
		exit(1);
	}

	int r, c;
	for (r = 0; r < height; r++)
		for (c = 0; c < height; c++)
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
		dprint("NULL_PTR on free_matrix = %p\n", mat_pointer);
	}
}

void mat_print(matrix mat) {
	if (!&mat) {
		dprint("EMPTY MATRIX RECEIVED!\n");
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
	if (!a || !b) {
		dprint("MATRIX %c is NULL!\n", (a ? 'b' : 'a'));
		return NULL;
	}
	if ((a->width != b->width) || (a->height != b->height)) {
		dprint("IMPROPER MATRIX CALCULATION! - PLUS\n");
		return NULL;
	}
	matrix* result = (matrix*)calloc(1, sizeof(matrix));
	if (result == NULL) {
		dprint("CALLOC FAILED at mat_plus - matrix!\n");
		exit(1);
	}
	result->height = a->height;
	result->width = a->width;
	result->mat = (float*)calloc(a->height * a->width, sizeof(float));
	if (result->mat == NULL) {
		dprint("CALLOC FAILED at mat_plus - float!\n");
		exit(1);
	}
	memcpy(result->mat, a->mat, a->height * a->width * sizeof(float));

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
	if (!a || !b) {
		dprint("MATRIX %c is NULL!\n", (a ? 'b' : 'a'));
		return NULL;
	}
	if ((a->width != b->width) || (a->height != b->height)) {
		dprint("IMPROPER MATRIX CALCULATION! - MINUS\n");
		return NULL;
	}
	matrix* result = (matrix*)calloc(1, sizeof(matrix));
	if (result == NULL) {
		dprint("CALLOC FAILED at mat_minus!\n");
		exit(1);
	}
	result->height = a->height;
	result->width = a->width;
	result->mat = (float*)calloc(a->height * a->width, sizeof(float));
	if (result->mat == NULL) {
		dprint("CALLOC FAILED at mat_minus - float!\n");
		exit(1);
	}
	memcpy(result->mat, a->mat, a->height * a->width * sizeof(float));

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
		dprint("MATRIX a is NULL!\n");
		return NULL;
	}
	matrix* result = (matrix*)calloc(1, sizeof(matrix));
	if (result == NULL) {
		dprint("CALLOC FAILED at mat_scala_multiply!\n");
		exit(1);
	}
	result->height = a->height;
	result->width = a->width;
	result->mat = (float*)calloc(a->height * a->width, sizeof(float));
	if (result->mat == NULL) {
		dprint("CALLOC FAILED at mat_scala_multiply - float!\n");
		exit(1);
	}
	memcpy(result->mat, a->mat, a->height * a->width * sizeof(float));

	int index;
	for (index = 0; index < result->height * result->width; index++)
		result->mat[index] *= b;
	return result;
}
matrix* mat_scala_divide(matrix* a, float b) {
	if (!a) {
		dprint("MATRIX a is NULL!\n");
		return NULL;
	}
	matrix* result = (matrix*)calloc(1, sizeof(matrix));
	if (result == NULL) {
		dprint("CALLOC FAILED at mat_scala_divide!\n");
		exit(1);
	}
	result->height = a->height;
	result->width = a->width;
	result->mat = (float*)calloc(a->height * a->width, sizeof(float));
	if (result->mat == NULL) {
		dprint("CALLOC FAILED at mat_scala_divide - float!\n");
		exit(1);
	}
	memcpy(result->mat, a->mat, a->height * a->width * sizeof(float));

	int index;
	for (index = 0; index < result->height * result->width; index++)
		result->mat[index] /= b;
	return result;
}
matrix* mat_multiply(matrix* a, matrix* b) {
	if (!a || !b) {
		dprint("MATRIX %c is NULL!\n", (a ? 'b' : 'a'));
		return NULL;
	}
	if (a->width != b->height) {
		dprint("IMPROPER MATRIX CALCULATION! - MULTIPLY\n");
		return NULL;
	}

	matrix* result = (matrix*)calloc(1, sizeof(matrix));
	if (result == NULL) {
		dprint("CALLOC FAILED at mat_multiply!\n");
		exit(1);
	}
	result->height = a->height;
	result->width = b->width;
	result->mat = (float*)calloc(result->height * result->width, sizeof(float));
	if (result->mat == NULL) {
		dprint("CALLOC FAILED at mat_multiply - float!\n");
		exit(1);
	}
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
	if (!a || !b) {
		dprint("MATRIX %c is NULL!\n", (a ? 'b' : 'a'));
		return NULL;
	}
	if (a->width != a->height || a->height != b->width || b->width != b->height) {
		dprint("WARNING : IMPROPER MATRIX CALCULATION! - SQUARE MATRIX MULTIPLY\n");
		return mat_multiply(a, b);
	}
	int r, c;
	matrix* result = (matrix*)calloc(1, sizeof(matrix));
	if (result == NULL) {
		dprint("CALLOC FAILED at mat_square_multiply - matrix!\n");
		exit(1);
	}
	result->height = a->height;
	result->width = b->width;
	result->mat = (float*)calloc(result->height * result->width, sizeof(float));
	if (result->mat == NULL) {
		dprint("CALLOC FAILED at mat_square_multiply - float!\n");
		exit(1);
	}
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
	if (!a || !b) {
		dprint("MATRIX %c is NULL!\n", (a ? 'b' : 'a'));
		return NULL;
	}
	// TODO : Let's make this function work!
	dprint("ERROR : mat_divide isn't implemented yet!\n");
	exit(2);
}

matrix* mat_transpose(matrix* target) {
	NULL_CHECK(target);
	if (target->height != target->width) 
		dprint("ERROR : Matrix is not square!\n");
	if (target->height > 4)
		dprint("ERROR : mat_transpose function's max support size is 4x4\n");

	matrix* result = create_identity_matrix(target->height, target->width);
	memcpy(result->mat, target->mat, sizeof(float) * target->height * target->width);

	float* mat = result->mat;
	float tmp;
	if (target->height == 4) {
		tmp = mat[1]; mat[1] = mat[4]; mat[4] = tmp;
		tmp = mat[2]; mat[2] = mat[8]; mat[8] = tmp;
		tmp = mat[3]; mat[3] = mat[12]; mat[12] = tmp;
		tmp = mat[6]; mat[6] = mat[9]; mat[9] = tmp;
		tmp = mat[7]; mat[7] = mat[13]; mat[13] = tmp;
		tmp = mat[11]; mat[11] = mat[14]; mat[14] = tmp;
	}
	else if (target->height == 3) {
		tmp = mat[1]; mat[1] = mat[3]; mat[3] = tmp;
		tmp = mat[2]; mat[2] = mat[6]; mat[6] = tmp;
		tmp = mat[5]; mat[5] = mat[7]; mat[7] = tmp;
	}
	else if (target->height == 2) {
		tmp = mat[1]; mat[1] = mat[2]; mat[2] = tmp;
	}

	return result;
}

matrix* mat_inverse(matrix* target) {
	NULL_CHECK(target);
	if (target->height != target->width)
		dprint("ERROR : Matrix is not square!\n");
	if (target->height > 4)
		dprint("ERROR : mat_inverse function's max support size is 4x4\n");

	matrix* result = create_identity_matrix(target->height, target->width);

	float* m = target->mat;
	float* inv = result->mat;
	double det = 0;
	if (target->height == 4) {
		// Source got from MESA implementation of the GLU library
		inv[0] = m[5] * m[10] * m[15] -
			m[5] * m[11] * m[14] -
			m[9] * m[6] * m[15] +
			m[9] * m[7] * m[14] +
			m[13] * m[6] * m[11] -
			m[13] * m[7] * m[10];

		inv[4] = -m[4] * m[10] * m[15] +
			m[4] * m[11] * m[14] +
			m[8] * m[6] * m[15] -
			m[8] * m[7] * m[14] -
			m[12] * m[6] * m[11] +
			m[12] * m[7] * m[10];

		inv[8] = m[4] * m[9] * m[15] -
			m[4] * m[11] * m[13] -
			m[8] * m[5] * m[15] +
			m[8] * m[7] * m[13] +
			m[12] * m[5] * m[11] -
			m[12] * m[7] * m[9];

		inv[12] = -m[4] * m[9] * m[14] +
			m[4] * m[10] * m[13] +
			m[8] * m[5] * m[14] -
			m[8] * m[6] * m[13] -
			m[12] * m[5] * m[10] +
			m[12] * m[6] * m[9];

		inv[1] = -m[1] * m[10] * m[15] +
			m[1] * m[11] * m[14] +
			m[9] * m[2] * m[15] -
			m[9] * m[3] * m[14] -
			m[13] * m[2] * m[11] +
			m[13] * m[3] * m[10];

		inv[5] = m[0] * m[10] * m[15] -
			m[0] * m[11] * m[14] -
			m[8] * m[2] * m[15] +
			m[8] * m[3] * m[14] +
			m[12] * m[2] * m[11] -
			m[12] * m[3] * m[10];

		inv[9] = -m[0] * m[9] * m[15] +
			m[0] * m[11] * m[13] +
			m[8] * m[1] * m[15] -
			m[8] * m[3] * m[13] -
			m[12] * m[1] * m[11] +
			m[12] * m[3] * m[9];

		inv[13] = m[0] * m[9] * m[14] -
			m[0] * m[10] * m[13] -
			m[8] * m[1] * m[14] +
			m[8] * m[2] * m[13] +
			m[12] * m[1] * m[10] -
			m[12] * m[2] * m[9];

		inv[2] = m[1] * m[6] * m[15] -
			m[1] * m[7] * m[14] -
			m[5] * m[2] * m[15] +
			m[5] * m[3] * m[14] +
			m[13] * m[2] * m[7] -
			m[13] * m[3] * m[6];

		inv[6] = -m[0] * m[6] * m[15] +
			m[0] * m[7] * m[14] +
			m[4] * m[2] * m[15] -
			m[4] * m[3] * m[14] -
			m[12] * m[2] * m[7] +
			m[12] * m[3] * m[6];

		inv[10] = m[0] * m[5] * m[15] -
			m[0] * m[7] * m[13] -
			m[4] * m[1] * m[15] +
			m[4] * m[3] * m[13] +
			m[12] * m[1] * m[7] -
			m[12] * m[3] * m[5];

		inv[14] = -m[0] * m[5] * m[14] +
			m[0] * m[6] * m[13] +
			m[4] * m[1] * m[14] -
			m[4] * m[2] * m[13] -
			m[12] * m[1] * m[6] +
			m[12] * m[2] * m[5];

		inv[3] = -m[1] * m[6] * m[11] +
			m[1] * m[7] * m[10] +
			m[5] * m[2] * m[11] -
			m[5] * m[3] * m[10] -
			m[9] * m[2] * m[7] +
			m[9] * m[3] * m[6];

		inv[7] = m[0] * m[6] * m[11] -
			m[0] * m[7] * m[10] -
			m[4] * m[2] * m[11] +
			m[4] * m[3] * m[10] +
			m[8] * m[2] * m[7] -
			m[8] * m[3] * m[6];

		inv[11] = -m[0] * m[5] * m[11] +
			m[0] * m[7] * m[9] +
			m[4] * m[1] * m[11] -
			m[4] * m[3] * m[9] -
			m[8] * m[1] * m[7] +
			m[8] * m[3] * m[5];

		inv[15] = m[0] * m[5] * m[10] -
			m[0] * m[6] * m[9] -
			m[4] * m[1] * m[10] +
			m[4] * m[2] * m[9] +
			m[8] * m[1] * m[6] -
			m[8] * m[2] * m[5];

		det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	}
	else if (target->height == 3) {
		eprint("Not Implemented, I'm too lazy to implement 3x3");
	}
	else if (target->height == 2) {
		// Dear god, this is much easier than 3x3 or god damn 4x4
		inv[0] = m[3];
		inv[1] = -m[1];
		inv[2] = -m[2];
		inv[3] = m[0];

		det = m[0] * m[3] - m[1] * m[2];
	}
	else { return NULL; }

	if (det == 0) {
		free_matrix(&result);
		return NULL;
	}

	int z = 0;
	for (z; z < target->height * target->width; z++) inv[z] /= det;
	return result;

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
		1.0f, 2.0f, 3.0f, 4.0f,
		5.0f, 6.0f, 7.0f, 8.0f,
		9.0f, 10.0f, 11.0f, 12.0f,
		13.0f, 14.0f, 15.0f, 16.0f);
	matrix* tmp_mat_2 = create_matrix(4, 4,
		1.0f, 2.0f, 3.0f, 4.0f,
		5.0f, 6.0f, 7.0f, 8.0f,
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
		1.0f, 2.0f,
		3.0f, 4.0f,
		5.0f, 6.0f,
		7.0f, 8.0f);
	matrix* tmp_mat_b = create_matrix(2, 5,
		1.0f, 2.0f, 3.0f, 4.0f, 5.0f,
		6.0f, 7.0f, 8.0f, 9.0f, 10.0f);
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