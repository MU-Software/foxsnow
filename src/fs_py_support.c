#include "fs_py_support.h"

PyObject *pConModule, *pConObj, *pConPushMtd;

bool FS_PyConsole_init() {
    PyObject *pName = PyUnicode_DecodeFSDefault("inapp_console");
    pConModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pConModule != NULL) {
        pConObj = PyObject_GetAttrString(pConModule, "fs_py_con");
        if (pConObj == NULL) { // Console Object get failure
            fprintf(stderr, "Failed to get Python Console Object\n");
            return 1;
        }
    } else { // Console Module load failure
        PyErr_Print();
        fprintf(stderr, "Failed to load Python Console\n");
        return 1;
    }

    pConPushMtd = PyObject_GetAttrString(pConObj, "push");
    if (pConPushMtd == NULL || !PyCallable_Check(pConModule)) {
        // Console push method get failure
        fprintf(stderr, "Failed to load Python Console\n");
        return 1;
    }
}

PyObject* FS_PyConsole_push(char* input_str) {
    if (PyCallable_Check(pConModule)) {
        PyObject *result;
        PyObject *args = PyTuple_New(1);
        PyObject *tmp_input_str = PyUnicode_FromString(input_str);
        if (PyTuple_SetItem(args, 0, tmp_input_str)){
                fprintf(stderr, "Error while setting input string for pushing\n");
                Py_DECREF(args);
                Py_DECREF(tmp_input_str);
                return NULL;
        }

        result = PyObject_CallObject(pConPushMtd, args);
        Py_DECREF(args);
        Py_DECREF(tmp_input_str);

        if (PyErr_Occurred()) {
            PyErr_Print();
            fprintf(stderr,"Console Push failed\n");
            Py_XDECREF(result);
            return NULL;
        }
        return result;
    } else { // Module, Console Object, Push method failure        
        if (pConModule == NULL || pConObj == NULL) {
            fprintf(stderr,
                    "Python console push called before %s initialize\n",
                    (pConModule != NULL ? "Console Object" : "Console Module"));
            return NULL;
        } else if (pConPushMtd == NULL) {
            fprintf(stderr, "Python console push method did not initialized\n");
            return NULL;
        } else {
            fprintf(stderr, "Python console push method cannot be called, FATAL_ERROR_CONSOLE\n");
            return NULL;
        }
    }
}