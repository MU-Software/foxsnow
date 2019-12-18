#include "3d_obj.h"

int loadOBJ(char* filename, int* vert_size, float** vert_arr, int** index_size, int* index_arr) {
    FILE *file_pointer = fopen(filename, "r");
    if (file_pointer == NULL) {
        printf("OBJ_LOADER > OBJ file load Failed.\n");
        return 1;
    }

    int line_counter = 0;
    int len_vertex = 0,
        len_normal = 0,
        len_texcoord = 0,
        len_index = 0;
    char str_buf[2048] = { 0 };
    for (line_counter=0; !feof(file_pointer); line_counter++) {
        // Read one line and remove \n character.
		fgets(str_buf, (int)sizeof(str_buf), file_pointer);
		strtok(str_buf, "\n");
        if (!(strcmp(str_buf, ""))) continue;

        // We'll only count the length of vertex, normal, texcoord, index this time
        // so that we can make proper size of array.
        if (str_buf[0] == 'v') {
            int len_slice = 1;
            char type_minor = str_buf[1]; // This is because strtok will corrupt original string.
            char* tmp_str_buf = strtok(str_buf, " ");
            for (len_slice; strtok(NULL, " "); len_slice++);
            if (type_minor == ' ') len_vertex += (len_slice - 1);
            else if (type_minor == 'n') len_normal += (len_slice - 1);
            else if (type_minor == 't') len_texcoord += (len_slice - 1);
        }
        else if (str_buf[0] == 'f') {
            int len_slice = 1;
            char* tmp_str_buf = strtok(str_buf, " ");
            for (len_slice; strtok(NULL, " "); len_slice++);
            len_index += (len_slice - 1);
        }
        memset(str_buf, 0, sizeof(str_buf));
	}
    printf("OBJ_LOADER > Read %d lines\n", line_counter);
    printf("OBJ_LOADER > Vertex = %d, TexCoord = %d, Normal = %d, Index = %d\n",
           len_vertex, len_texcoord, len_normal, len_index);
    rewind(file_pointer);

    // Create data array
    int *arr_index = (int*)calloc(len_index, sizeof(int));
    float *arr_vertex = (float*)calloc(len_vertex, sizeof(float));
    float *arr_texcoord = (float*)calloc(len_texcoord, sizeof(float));
    float *arr_normal = (float*)calloc(len_normal, sizeof(float));

    int index_arr_index = 0,
        index_arr_vertex = 0,
        index_arr_texcoord = 0,
        index_arr_normal = 0;

    for (line_counter=0; !feof(file_pointer); line_counter++) {
		fgets(str_buf, (int)sizeof(str_buf), file_pointer);
		strtok(str_buf, "\n");
        if (!(strcmp(str_buf, ""))) continue;

        if (str_buf[0] == 'v') {
            float* target_arr;
            int* target_arr_index;
            char target_type[4];
            if (str_buf[1] == ' ') {
                target_arr = arr_vertex;
                target_arr_index = &index_arr_vertex;
                strcpy(target_type, "VERT");
            }
            else if (str_buf[1] == 'n') {
                target_arr = arr_normal;
                target_arr_index = &index_arr_normal;
                strcpy(target_type, "NORM");
            }
            else if (str_buf[1] == 't') {
                target_arr = arr_texcoord;
                target_arr_index = &index_arr_texcoord;
                strcpy(target_type, "UV");
            }

            // Now, We'll record data on array.
            char* target_slice = strtok(str_buf, " ");
            int len_slice = 0;
            while (target_slice) {
                // First slice should be delimiter, so ignore it.
                if (!len_slice) {
                    len_slice++;
                    target_slice = strtok(NULL, " ");
                    continue;
                }

                // Check if this slice is float
                int slice_str_len = 0;
                for (slice_str_len; slice_str_len <= strlen(target_slice)-1; slice_str_len++) {
                    char target_char = target_slice[slice_str_len];
                    if (!('0' <= target_char && target_char <= '9') && !(target_char == '.') && !(target_char == '-')) {
                        printf("OBJ_LOADER > Wrong %s data(VALUE IS NOT FLOAT) on line %d.\n",
                               target_type, line_counter);
                        goto error_after_calloc;
                    }
                }

                // Insert data to arr_vertex
                target_arr[*target_arr_index] = strtof(target_slice, NULL);
                (*target_arr_index)++;
                
                // Go to next slice
                len_slice++;
                target_slice = strtok(NULL, " ");
            }

            // If count of slices are not 4 or 5,
            // We can assumes that this line has more data than we expect. 
            if (!(len_slice == 4 || len_slice == 5)) {
                printf("OBJ_LOADER > Wrong %s data(TOO MUCH DATA IN LINE) on line %d.\n",
                       target_type, line_counter);
                goto error_after_calloc;
            }
        }
        else if (str_buf[0] == 'f') {
            // Index recording.
            char* token_slice_ptr1, token_slice_ptr2;
            char* target_slice_1 = strtok_r(str_buf, " ", &token_slice_ptr1);
            int len_slice = 0;
            while (target_slice_1) {
                // First slice should be delimiter, so ignore it.
                if (!len_slice) {
                    len_slice++;
                    target_slice_1 = strtok_r(NULL, " ", &token_slice_ptr1);
                    continue;
                }

                // Insert data to arr_vertex
                if (strchr(target_slice_1, '/') == NULL) {
                    arr_index[index_arr_index] = atoi(target_slice_1)-1;
                    index_arr_index++;
                }
                else {
                    // We won't support texcoord and normal this time.
                    arr_index[index_arr_index] = atoi(strtok_r(target_slice_1, "/", &token_slice_ptr2));
                    index_arr_index++;
                }
                
                // Go to next slice
                len_slice++;
                target_slice_1 = strtok_r(NULL, " ", &token_slice_ptr1);
            }

            // If count of slices are not 4,(We won't support n-gon this time.)
            // We can assumes that this line has more data than we expect. 
            if (len_slice != 4) {
                printf("OBJ_LOADER > Wrong FRAG data(TOO MUCH DATA IN LINE) on line %d.\n", line_counter);
                goto error_after_calloc;
            }
        }
        memset(str_buf, 0, sizeof(str_buf));
	}
    *vert_arr = arr_vertex;
    *index_arr = arr_index;

    *vert_size = len_vertex;
    *index_size = len_index;

    return 0;

    error_after_calloc:
        free(arr_index);
        free(arr_vertex);
        free(arr_texcoord);
        free(arr_normal);
        return 2;
}