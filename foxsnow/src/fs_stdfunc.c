#include "fs_stdfunc.h"

void adjustWindowSize(SHORT x, SHORT y) {
	struct SMALL_RECT test;

	HANDLE hStdout;
	COORD coord;
	BOOL ok;

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	coord.X = x;
	coord.Y = y;
	ok = SetConsoleScreenBufferSize(hStdout, coord);

	test.Left = 0;
	test.Top = 0;
	test.Right = coord.X - 1;
	test.Bottom = coord.Y - 1;

	SetConsoleWindowInfo(hStdout, ok, &test);
}

unsigned long long get_current_time() {
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	unsigned long long tt = ft.dwHighDateTime;
	tt <<= 32;
	tt |= ft.dwLowDateTime;
	tt /= 10;
	tt -= 11644473600000000ULL;
	return tt;
}

unsigned long long get_elapsed_time() {
	static unsigned long long start_time = 0;
	if (!start_time) start_time = get_current_time();

	return get_current_time() - start_time;
}


char* fs_file_to_mem(const char* path) {
	char* str_storage = NULL;
	FILE* fp = fopen(path, "r");
	if (fp != NULL) {
		if (fseek(fp, 0L, SEEK_END) == 0) {
			/* Get the size of the file. */
			long file_size = ftell(fp);
			if (file_size == -1) {
				dprint("Error while calculating file size! C");
				fclose(fp);
				return NULL;
			}
			else if (!file_size) {
				dprint("File is empty!");
				fclose(fp);
				return NULL;
			}

			str_storage = (char*)calloc((file_size + 1), sizeof(char));
			if (str_storage == NULL) {
				dprint("CALLOC FAILED on fs_file_to_mem!\n");
				exit(1);
			}

			if (fseek(fp, 0L, SEEK_SET)) {
				dprint("Error while calculating file size! B");
				fclose(fp);
				return NULL;
			}

			/* Read the entire file into memory. */
			size_t newLen = fread(str_storage, sizeof(char), file_size, fp);
			if (ferror(fp) != 0) {
				dprint("Error while reading file");
			}
			else { str_storage[newLen++] = '\0'; /* Just to be safe. */ }
		}
		else {
			dprint("Error while calculating file size! A");
			fclose(fp);
			return NULL;
		}
		fclose(fp);
	}
	else {
		printf("Cannot open file! ERROR_CODE=%d(%s)\n", errno, strerror(errno));
		return NULL;
	}
	return str_storage;
}

unsigned long hash(unsigned const char* str) {
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c;

	return hash;
}

double getRadian(double degree) {
	return degree * (M_PI / 180.0);
}

void normalizeVector(float* vec) {
	NULL_CHECK(vec);
	float w = sqrtf(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] /= w;
	vec[1] /= w;
	vec[2] /= w;
}
