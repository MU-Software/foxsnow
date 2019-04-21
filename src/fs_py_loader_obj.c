#include <GL/glew.h>
#include <Python/Python.h>

#include "fs_py_support.h"

GLfloat* FSloadOBJ(char* filename) {
    PyObject* args;
    PyObject* tmp_str;
    PyObject* result;
    float* test_1;

    tmp_str = PyUnicode_FromString(filename);
    //args = PyTuple_New(0);
    args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, tmp_str);
    //PyTuple_SetItem(args, 1, test_1);
    //PyTuple_SetItem(args, 2, test_1);

    PyObject *pName, *pModule, *pFunc;

    printf("ALIVE_A\n");
    pName = PyUnicode_DecodeFSDefault("jar_obj");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    printf("ALIVE_B\n");

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, "load_obj");
        printf("ALIVE_C\n");
        if (pFunc && PyCallable_Check(pFunc)) {
            printf("ALIVE_C_1\n");
            result = PyObject_CallObject(pFunc, args);
            printf("ALIVE_D\n");
            // IMPORTANT : DO Py_DECREF(argument) after function ends.
            // Py_DECREF(argument);
            if (PyErr_Occurred()) {
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                Py_XDECREF(result);
            }
            printf("ALIVE_E\n");
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
    printf("ALIVE_F\n");
}