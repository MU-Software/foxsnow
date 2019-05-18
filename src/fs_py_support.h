#include <stdbool.h>
#include <Python/Python.h>

bool FS_PyConsole_init();
PyObject* FS_PyConsole_push(char* input_str);