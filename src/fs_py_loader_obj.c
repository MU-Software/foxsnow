#include <GL/glew.h>
#include <Python/Python.h>

#include "fs_py_support.h"

GLfloat* FSloadOBJ(char* filename) {
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
        printf("ALIVE_C\n");
        if (pFunc && PyCallable_Check(pFunc)) {
            printf("ALIVE_C_1\n");
            result = PyObject_CallObject(pFunc, args);
            printf("ALIVE_D\n");
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

    float* verts = NULL;
    float* index = NULL; 
    int result_len = PyTuple_Size(result);
    printf("Number of returned value : %d\n", result_len);
    if (result_len == 4) {
        int arr_size = (int)PyLong_AsLong(PyTuple_GetItem(result, 0));
        float* tmp_arr = PyTuple_GetItem(result, 1);
        printf("result_1 = %f\n", *tmp_arr);

        verts = malloc(arr_size);
        memcpy(verts, PyTuple_GetItem(result, 1), arr_size);
        printf("result = %f\n", verts[100]);
    }
}