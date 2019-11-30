#include "fs_py_support.h"

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
    if (pConModule != NULL || pConObj != NULL) {
        PyObject *result;
        PyObject *args = PyTuple_New(1);
        PyObject *tmp_input_str = PyUnicode_FromString(input_str);
        if (PyTuple_SetItem(args, 0, tmp_input_str)){
                printf("Error while setting input string for pushing\n");
                Py_DECREF(args);
                Py_DECREF(tmp_input_str);
                return NULL;
        }
        PyObject *tmp_mtd_str = PyUnicode_FromString((char*)"push");

        printf("DEBUG_A 0\n");
        printf("%p %p, %p", pConObj, tmp_mtd_str, args);
        result = PyObject_CallMethodObjArgs(pConObj, tmp_mtd_str, args);
        printf("DEBUG_A 1\n");

        Py_DECREF(args);
        printf("DEBUG_A 2\n");
        Py_DECREF(tmp_input_str);
        printf("DEBUG_A 3\n");
        if (result == NULL) fprintf(stderr, "Bad Console Push Result\n");
        printf("DEBUG_A 4\n");

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