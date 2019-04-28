#include "fs_py_support.h"

int FS_PYTHON_FUNC(char* module, char* function, PyObject* argument, PyObject* pValue) {
    PyObject *pName, *pModule, *pFunc;

    printf("ALIVE_A\n");
    pName = PyUnicode_DecodeFSDefault(module);
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    printf("ALIVE_B\n");

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, function);
        printf("ALIVE_C\n");
        if (pFunc && PyCallable_Check(pFunc)) {
            printf("ALIVE_C_1\n");
            pValue = PyObject_CallObject(pFunc, argument);
            printf("ALIVE_D\n");
            // IMPORTANT : DO Py_DECREF(argument) after function ends.
            // Py_DECREF(argument);
            if (PyErr_Occurred()) {
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                Py_XDECREF(pValue);
            }
            printf("ALIVE_E\n");
        }
        else { // Function load failure
            if (PyErr_Occurred()) PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", function);
        }
        Py_XDECREF(pFunc);
        Py_XDECREF(pModule);
    } else { // Module load failure
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", module);
        return 1;
    }
    printf("ALIVE_F");
}