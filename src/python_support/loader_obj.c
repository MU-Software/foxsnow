#include "loader_obj.h"

GLfloat* FSloadOBJ(char* filename, int* vert_size, float** vert_arr, int** index_size, int* index_arr) {
    PyObject* args;
    PyObject* tmp_str;
    PyObject* result;

    tmp_str = PyUnicode_FromString(filename);
    args = PyTuple_New(1);
    printf("py_tuple_arg_set_1 : %d\n", PyTuple_SetItem(args, 0, tmp_str));

    PyObject *pName, *pModule, *pFunc;

    pName = PyUnicode_DecodeFSDefault("jar_obj");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, "load_obj");
        if (pFunc && PyCallable_Check(pFunc)) {
            result = PyObject_CallObject(pFunc, args);
            Py_DECREF(tmp_str);
            // IMPORTANT : DO Py_DECREF(argument) after function ends.
            // Py_DECREF(argument);
            if (PyErr_Occurred()) {
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                Py_XDECREF(result);
            }
        }
        else { // Function load failure
            if (PyErr_Occurred()) PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", "load_obj");
        }
        Py_XDECREF(pFunc);
        Py_XDECREF(pModule);
    } else { // Module load failure
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", "jar_obj");
        return 1;
    }

    int result_len = PyTuple_Size(result);
    if (result_len == 4) {
        *vert_size = (int)PyLong_AsLong(PyTuple_GetItem(result, 1));
        *vert_arr   = (float*)PyLong_AsLong(PyTuple_GetItem(result, 0));
        printf("vertex | size = %d, pointer = %p | AT C\n", *vert_size, vert_arr);

        *index_size = (int)PyLong_AsLong(PyTuple_GetItem(result, 3));
        *index_arr   = (int*)PyLong_AsLong(PyTuple_GetItem(result, 2));
        printf("index  | size = %d, pointer = %p | AT C\n", *index_size, index_arr);
    }

    // int teapot_vert_size, teapot_index_size;
    // float* teapot_vertex_array;
    // int* teapot_index_array;
    //FSloadOBJ("resources/teapot.obj", &teapot_vert_size,  &teapot_vertex_array,
    //                                  &teapot_index_size, &teapot_index_array);


}