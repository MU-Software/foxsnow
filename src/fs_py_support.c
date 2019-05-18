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

    // pConPushMtd = PyObject_GetAttrString(pConObj, "push");
    // if (pConPushMtd == NULL || !PyCallable_Check(pConModule)) {
    //     // Console push method get failure
    //     fprintf(stderr, "Failed to load Python Console\n");
    //     return 1;
    // }
}

PyObject* FS_PyConsole_push(char* input_str) {
    if (pConModule != NULL || pConObj != NULL) {
        PyObject *result;
        PyObject *args = PyTuple_New(1);
        PyObject *tmp_input_str = PyUnicode_FromString(input_str);
        if (PyTuple_SetItem(args, 0, tmp_input_str)){
                fprintf(stderr, "Error while setting input string for pushing\n");
                Py_DECREF(args);
                Py_DECREF(tmp_input_str);
                return NULL;
        }
        PyObject *tmp_mtd_str = PyUnicode_FromString((char*)"push");

        result = PyObject_CallMethodObjArgs(pConObj, tmp_mtd_str, args);
        Py_DECREF(args);
        Py_DECREF(tmp_input_str);
        if (result == NULL) fprintf(stderr, "Bad Console Push Result\n");

        if (PyErr_Occurred()) {
            PyErr_Print();
            fprintf(stderr,"Console Push failed\n");
            Py_XDECREF(result);
            return NULL;
        }
        return result;
    } else { // Module, Console Object, Push method failure        
        fprintf(stderr,
                "Python console push called before %s initialize\n",
                (pConModule != NULL ? "Console Object" : "Console Module"));
        return NULL;
    }
}