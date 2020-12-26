#pragma once
#ifndef FS_LIB_DEBUG_PRINT
#define FS_LIB_DEBUG_PRINT

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <time.h>

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifdef _DEBUG
#define dprint(...) debug_print(stdout,__FILENAME__,__LINE__,__func__,__VA_ARGS__)
#define eprint(...) debug_print(stderr,__FILENAME__,__LINE__,__func__,__VA_ARGS__)
#define ALLOC_FAILCHECK(mem) if(!mem){eprint("CALLOC FAILED!\n");}
#define NULL_CHECK(mem) if(!mem) {eprint("NULL POINTER EXCEPTION!\n");}
#else
#define dprint(...) dummy_print(__VA_ARGS__)
#define eprint(...) dummy_print(__VA_ARGS__)
#define ALLOC_FAILCHECK(mem) {}
#define NULL_CHECK(mem) {}
#endif

FILE* log_file;

void debug_print(
    FILE* stream,
    const char* filename,
    int line,
    const char* callername,
    const char* target_str, ...);
void dummy_print(const char* dummy, ...);

#endif