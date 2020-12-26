#include "fs_debug_print.h"

void debug_print(
    FILE* stream,
    const char* filename,
    int line,
    const char* callername,
    const char* target_str, ...) {
    static bool did_fp_errmsg_printed = false;
    if (log_file == NULL) {
        if (!did_fp_errmsg_printed) {
            did_fp_errmsg_printed = true;
            dprint("COULD NOT OPEN LOG FILE!(maybe disabled?)\n");
        }
    }

    time_t ltime;
    ltime = time(NULL);
    char* target_time_str = asctime(localtime(&ltime));
    target_time_str[strlen(target_time_str) - 1] = '\0';
    fprintf(stream, "%s %s[%3d] @%s >> ", target_time_str, filename, line, callername);
    if (log_file) {
        fprintf(log_file, "%s %s[%3d] @%s >> ", target_time_str, filename, line, callername);
        fflush(log_file);
    }

    va_list arglist, arglist2;
    va_start(arglist, target_str);
    va_copy(arglist2, arglist);
    vfprintf(stream, target_str, arglist);
    if (log_file)
        vfprintf(log_file, target_str, arglist2);
    va_end(arglist);

    /*int z = 0;
    for (z = 0; target_str && strlen(target_str) >= z; z++) {
        if (target_str[z] == '\0' && target_str[z - 1] != '\n') {
            fprintf(stream, "\n");
            if (log_file) {
                fprintf(log_file, "\n");
                fflush(log_file);
            }
        }
    }*/

    if (stream == stderr) {
        debug_print(stdout, filename, line, callername, "Program exited abnormally.");
        if (log_file) {
            fclose(log_file);
            log_file = NULL;
        }
        exit(1);
    }
}

void dummy_print(const char* dummy, ...) {
    return;
}
