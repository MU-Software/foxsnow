#include "fs_stdfunc.h"

unsigned int HashCode(const char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = 65599 * hash + str[0];
        str++;
    }
    return hash ^ (hash >> 16);
}

char* get_exe_path(char* path, int size) {

    #ifdef _WIN32
    GetModuleFileName(NULL, path, size);
    #elif __linux__
    readlink("/proc/self/exe", path, size);
    #elif  __APPLE__ 
    readlink("/proc/curproc/file", path, size);
    #elif __FreeBSD__
    readlink("/proc/curproc/file", path, size);
    #endif

    return path;
}

char* file_to_mem(char* path) {
	char *str_storage = NULL;
	FILE *fp = fopen(path, "r");
	if (fp != NULL) {
		if (fseek(fp, 0L, SEEK_END) == 0) {
			/* Get the size of the file. */
			long file_size = ftell(fp);
			if (file_size == -1) {
				printf("Error while calculating file size! C");
				fclose(fp);
				return 0;
			} else if (!file_size) {
				printf("File is empty!");
				fclose(fp);
				return 0;
			}

			str_storage = malloc(sizeof(char) * (file_size + 1));

			if (fseek(fp, 0L, SEEK_SET)) {
				printf("Error while calculating file size! B");
				fclose(fp);
				return 0;
			}

			/* Read the entire file into memory. */
			size_t newLen = fread(str_storage, sizeof(char), file_size, fp);
			if (ferror(fp) != 0) {
				fputs("Error while reading file", stderr);
			} else { str_storage[newLen++] = '\0'; /* Just to be safe. */ }
		} else {
				printf("Error while calculating file size! A");
				fclose(fp);
				return 0;
		}
		fclose(fp);
	} else { printf("Cannot open file!"); return 0; }
	return str_storage;
}

char* str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}
