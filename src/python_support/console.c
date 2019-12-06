#include "console.h"

PyObject *pConModule, *pConObj;

bool FS_PyConsole_init() {
    PyObject *pName = PyUnicode_DecodeFSDefault("inapp_console");
    pConModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pConModule != NULL) {
        pConObj = PyObject_GetAttrString(pConModule, "fs_py_con");
        if (pConObj == NULL) { // Console Object get failure
            printf("Failed to get Python Console Object\n");
            return 1;
        }
    } else { // Console Module load failure
        PyErr_Print();
        printf("Failed to load Python Console Module\n");
        return 1;
    }
    return false;
}

PyObject* FS_PyConsole_push(char* input_str) {
    if (pConModule != NULL && pConObj != NULL) {
        PyObject *result;
        result = PyObject_CallMethod(pConObj, "push", "s", input_str);
        if (result == NULL) {
            printf("Bad Console Push Result\n");
            return NULL;
        }

        if (PyErr_Occurred()) {
            PyErr_Print();
            printf("Console Push failed\n");
            Py_XDECREF(result);
            return NULL;
        }
        return result;
    } else { // Module, Console Object, Push method failure        
        printf("Python console push called before %s initialize\n",
               (pConModule != NULL ? "Console Object" : "Console Module"));
        return NULL;
    }
}