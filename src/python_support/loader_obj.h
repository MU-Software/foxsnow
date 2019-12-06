#ifndef FS_PY_LOADER_OBJ
    #define FS_PY_LOADER_OBJ

    #include <GL/glew.h>
    #include <Python/Python.h>

    GLfloat* FSloadOBJ(char* filename, int* vert_size, float** vert_arr, int** index_size, int* index_arr);
#endif