#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifdef _WIN32
    #include "windows.h"
#endif

unsigned int HashCode(const char *str);
char* get_exe_path(char* path, int size);
char* file_to_mem(char* path);
char* str_replace(char *orig, char *rep, char *with);