#include "3d_obj.h"

FS_VCData* find_eaindex_from_list(list* src, const char* target) {
    list_element* current_element = src->head;
    while (current_element) {
        FS_VCData* current_element_data = (FS_VCData*)(current_element->data);
        if (!strcmp(current_element_data->cache_string, target)) return current_element_data;

        current_element = current_element->next;
    }
    return NULL;
}

void commit_point_cache_in_face(FS_FEIndex* face) {
    if (!face) return;
    int z = 0;
    for (z; z < 3; z++) {
        FS_EAIndex* point = &(face->p[z]);
        sprintf(point->cache_string, "%d/%d/%d", point->pos, point->uv, point->normal);
        point->cache_hash = hash(point->cache_string);
    }
}

float* list_to_heap_array_float(list* src, int count) {
    if (!src || !src->length) return NULL;
    float* result_array = (float*)calloc((src->length) * count, sizeof(float));
    if (!result_array) return NULL;

    int index = 0;
    list_element* target_element = src->head;
    while (target_element) {
        memcpy((void*)(result_array + (index * count)), target_element->data, sizeof(float) * count);
        index++;
        target_element = target_element->next;
    }
    return result_array;
}

int loadOBJ_new(char* filename, FS_LoaderOBJ_DataContainer* container) {
    FILE* file_pointer = fopen(filename, "r");
    if (file_pointer == NULL) {
        dprint("OBJ_LOADER > OBJ file load Failed.\n");
        return 1;
    }

    list* list_vertex = create_list();  // free checked
    list* list_texcoord = create_list();  // free checked
    list* list_normal = create_list();  // free checked

    list* obj_index = create_list();  // free checked

    // obj_type[0] => 't'ri or 'q'uad
    // obj_type[1] => 0b00 - 0b11, 0b[uv][norm]
    char obj_type[2] = { 0 };
    int face_count = 0;
    float max_radius = 0.0f;

    while (!feof(file_pointer)) {
        // Read one line and remove \n character.
        char str_buf[512] = { 0 };
        char* _tmp = NULL;
        fgets(str_buf, (int)sizeof(str_buf), file_pointer);
        _tmp = strtok(str_buf, "\n");
        if (!(strcmp(str_buf, ""))) continue;

        int num_read = 0;
        char prefix[3] = { 0 };
        float x, y, z;
        if ((num_read = sscanf(str_buf, "%2s %f %f %f", prefix, &x, &y, &z)) < 1) continue;

        switch (prefix[0]) {
        case 'v': {  // Vertex attributes parser
            switch (prefix[1]) {
            case '\0':  // Vertex position
                if (num_read != 4) continue;
                float* tmp_vert = (float*)calloc(3, sizeof(float));
                tmp_vert[0] = x, tmp_vert[1] = y, tmp_vert[2] = z;
                float current_radius = sqrt(x * x + y * y + z * z);
                if (current_radius > max_radius) max_radius = current_radius;
                list_append(list_vertex, tmp_vert, 0);
                break;
            case 't':  // Texture coordinate
                if (num_read != 3) continue;
                float* tmp_uv = (float*)calloc(2, sizeof(float));
                tmp_uv[0] = x, tmp_uv[1] = y;
                list_append(list_texcoord, tmp_uv, 0);
                break;
            case 'n':  // Normal vector
                if (num_read != 4) continue;
                float* tmp_norm = (float*)calloc(3, sizeof(float));
                tmp_norm[0] = x, tmp_norm[1] = y, tmp_norm[2] = z;
                list_append(list_normal, tmp_norm, 0);
                break;
            default:
                continue;
            }
        } break;
        case 'f': {  // Face parser
            int ind_pos[4] = { 0 };
            int ind_uv[4] = { 0 };
            int ind_norm[4] = { 0 };
            face_count++;
            // Handle quads (four vertices on one face)
            if (sscanf(str_buf, "%2s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",  // Quad with pos/uv/norm
                prefix,
                &ind_pos[0], &ind_uv[0], &ind_norm[0],
                &ind_pos[1], &ind_uv[1], &ind_norm[1],
                &ind_pos[2], &ind_uv[2], &ind_norm[2],
                &ind_pos[3], &ind_uv[3], &ind_norm[3]) == 13) {
                if (!obj_type[0]) obj_type[0] = 'q', obj_type[1] = 0b11;
                else if (obj_type[0] != 'q' || obj_type[1] != 0b11) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0], face_1->p[0].uv = ind_uv[0], face_1->p[0].normal = ind_norm[0];
                face_1->p[1].pos = ind_pos[1], face_1->p[1].uv = ind_uv[1], face_1->p[1].normal = ind_norm[1];
                face_1->p[2].pos = ind_pos[3], face_1->p[2].uv = ind_uv[3], face_1->p[2].normal = ind_norm[3];
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);

                FS_FEIndex* face_2 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_2);
                face_2->p[0].pos = ind_pos[1], face_2->p[0].uv = ind_uv[1], face_2->p[0].normal = ind_norm[1];
                face_2->p[1].pos = ind_pos[2], face_2->p[1].uv = ind_uv[2], face_2->p[1].normal = ind_norm[2];
                face_2->p[2].pos = ind_pos[3], face_2->p[2].uv = ind_uv[3], face_2->p[2].normal = ind_norm[3];
                commit_point_cache_in_face(face_2);
                list_append(obj_index, (void*)face_2, 0);
            }
            else if (sscanf(str_buf, "%2s %d//%d %d//%d %d//%d %d//%d",  // Quad with pos//norm
                prefix,
                &ind_pos[0], &ind_norm[0],
                &ind_pos[1], &ind_norm[1],
                &ind_pos[2], &ind_norm[2],
                &ind_pos[3], &ind_norm[3]) == 9) {
                if (!obj_type[0]) obj_type[0] = 'q', obj_type[1] = 0b01;
                else if (obj_type[0] != 'q' || obj_type[1] != 0b01) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0], face_1->p[0].normal = ind_norm[0];
                face_1->p[1].pos = ind_pos[1], face_1->p[1].normal = ind_norm[1];
                face_1->p[2].pos = ind_pos[3], face_1->p[2].normal = ind_norm[3];
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);

                FS_FEIndex* face_2 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_2);
                face_2->p[0].pos = ind_pos[1], face_2->p[0].normal = ind_norm[1];
                face_2->p[1].pos = ind_pos[2], face_2->p[1].normal = ind_norm[2];
                face_2->p[2].pos = ind_pos[3], face_2->p[2].normal = ind_norm[3];
                commit_point_cache_in_face(face_2);
                list_append(obj_index, (void*)face_2, 0);
            }
            else if (sscanf(str_buf, "%2s %d/%d %d/%d %d/%d %d/%d",  // Quad with pos/uv
                prefix,
                &ind_pos[0], &ind_uv[0],
                &ind_pos[1], &ind_uv[1],
                &ind_pos[2], &ind_uv[2],
                &ind_pos[3], &ind_uv[3]) == 9) {
                if (!obj_type[0]) obj_type[0] = 'q', obj_type[1] = 0b10;
                else if (obj_type[0] != 'q' || obj_type[1] != 0b10) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0], face_1->p[0].uv = ind_uv[0];
                face_1->p[1].pos = ind_pos[1], face_1->p[1].uv = ind_uv[1];
                face_1->p[2].pos = ind_pos[3], face_1->p[2].uv = ind_uv[3];
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);

                FS_FEIndex* face_2 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_2);
                face_2->p[0].pos = ind_pos[1], face_2->p[0].uv = ind_uv[1];
                face_2->p[1].pos = ind_pos[2], face_2->p[1].uv = ind_uv[2];
                face_2->p[2].pos = ind_pos[3], face_2->p[2].uv = ind_uv[3];
                commit_point_cache_in_face(face_2);
                list_append(obj_index, (void*)face_2, 0);
            }
            else if (sscanf(str_buf, "%2s %d %d %d %d",  // Quad with pos only
                prefix, &ind_pos[0], &ind_pos[1], &ind_pos[2], &ind_pos[3]) == 5) {
                if (!obj_type[0]) obj_type[0] = 'q', obj_type[1] = 0b00;
                else if (obj_type[0] != 'q' || obj_type[1] != 0b00) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0];
                face_1->p[1].pos = ind_pos[1];
                face_1->p[2].pos = ind_pos[3];
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);

                FS_FEIndex* face_2 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_2);
                face_2->p[0].pos = ind_pos[1];
                face_2->p[1].pos = ind_pos[2];
                face_2->p[2].pos = ind_pos[3];
                commit_point_cache_in_face(face_2);
                list_append(obj_index, (void*)face_2, 0);
            }


            // Handle triangles, this is normal. (four vertices on one face)
            else if (sscanf(str_buf, "%2s %d/%d/%d %d/%d/%d %d/%d/%d",  // Tri with pos/uv/norm
                prefix,
                &ind_pos[0], &ind_uv[0], &ind_norm[0],
                &ind_pos[1], &ind_uv[1], &ind_norm[1],
                &ind_pos[2], &ind_uv[2], &ind_norm[2]) == 10) {
                if (!obj_type[0]) obj_type[0] = 't', obj_type[1] = 0b11;
                else if (obj_type[0] != 't' || obj_type[1] != 0b11) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0], face_1->p[0].uv = ind_uv[0], face_1->p[0].normal = ind_norm[0];
                face_1->p[1].pos = ind_pos[1], face_1->p[1].uv = ind_uv[1], face_1->p[1].normal = ind_norm[1];
                face_1->p[2].pos = ind_pos[2], face_1->p[2].uv = ind_uv[2], face_1->p[2].normal = ind_norm[2];
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);
            }
            else if (sscanf(str_buf, "%2s %d//%d %d//%d %d//%d",  // Tri with pos//norm
                prefix,
                &ind_pos[0], &ind_norm[0],
                &ind_pos[1], &ind_norm[1],
                &ind_pos[2], &ind_norm[2]) == 7) {
                if (!obj_type[0]) obj_type[0] = 't', obj_type[1] = 0b01;
                else if (obj_type[0] != 't' || obj_type[1] != 0b01) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0], face_1->p[0].normal = ind_norm[0];
                face_1->p[1].pos = ind_pos[1], face_1->p[1].normal = ind_norm[1];
                face_1->p[2].pos = ind_pos[2], face_1->p[2].normal = ind_norm[2];
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);
            }
            else if (sscanf(str_buf, "%2s %d/%d %d/%d %d/%d",  // Tri with pos/uv
                prefix,
                &ind_pos[0], &ind_uv[0],
                &ind_pos[1], &ind_uv[1],
                &ind_pos[2], &ind_uv[2]) == 7) {
                if (!obj_type[0]) obj_type[0] = 't', obj_type[1] = 0b10;
                else if (obj_type[0] != 't' || obj_type[1] != 0b10) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0], face_1->p[0].uv = ind_uv[0];
                face_1->p[1].pos = ind_pos[1], face_1->p[1].uv = ind_uv[1];
                face_1->p[2].pos = ind_pos[2], face_1->p[2].uv = ind_uv[2];
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);
            }
            else if (sscanf(str_buf, "%2s %d %d %d",  // Tri with pos only
                prefix, &ind_pos[0], &ind_pos[1], &ind_pos[2]) == 4) {
                if (!obj_type[0]) obj_type[0] = 't', obj_type[1] = 0b00;
                else if (obj_type[0] != 't' || obj_type[1] != 0b00) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0];
                face_1->p[1].pos = ind_pos[1];
                face_1->p[2].pos = ind_pos[2];
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);
            }
        } break;
        case 't': {  // Custom texture load command
            char tmp_buf_1[512] = { 0 };
            char prefix[3] = { 0 };
            if (num_read = sscanf(str_buf, "%2s %s", prefix, tmp_buf_1) == 2) {
                TextureInfo* texture_load_result = loadPNG(tmp_buf_1, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
                if (texture_load_result) {
                    if (!container->texture_list) {
                        container->texture_list = create_list();
                    }
                    list_append(container->texture_list, texture_load_result, NULL);
                }
                else {
                    dprint("Texture load failed!\n");
                }
            }
        } break;
        case '#':  // Comment
            continue;
        default:
            continue;
        }
    }
    fclose(file_pointer);

    if (list_vertex->length == 0)  // list_vertex must not be empty.
        goto error_after_list_creation;

    // Now let's convert list to array for fast random accessing.
    float* arr_vertex = list_to_heap_array_float(list_vertex, 3);  // free checked
    float* arr_texcoord = list_to_heap_array_float(list_texcoord, 2);  // free checked
    float* arr_normal = list_to_heap_array_float(list_normal, 3);  // free checked

    // We need to associate those attributes to same index,
    // Because OpenGL does not support different indices for vertex attributes,
    // all vertex attributes have to use the same index.
    // So, We'll re-arranging those attributes to same index, and we will use RBtree here.
    // 1. We'll for-loop the vert/tex/normal index list and get each element.
    // 2. Stack it in order each attributes.
    // 3. Make string of parsed index and hash it.
    // 4. Save hash result and index in hash. Hash result will be the key of RBtree.
    //    In this way, we can get previously used vertices' index.
    // SO let's started!
    list* list_vp_ordered = create_list();  // [x, y, z], [x, y, z], [x, y, z], ...  // free checked
    list* list_uv_ordered = create_list();  // [u, v], [u, v], [u, v], ...  // free checked
    list* list_nm_ordered = create_list();  // [nx, ny, nz], [nx, ny, nz], [nx, ny, nz], ...  // free checked
    int* arr_element_buffer = (int*)calloc(face_count * 3, sizeof(int));
    ALLOC_FAILCHECK(arr_element_buffer);
    int arr_element_buffer_index = 0;

    rbtree_node* vertex_lookup = NULL;

    list_element* target_process_face_element = obj_index->head;
    while (target_process_face_element) {
        int vertex_index = 0;

        FS_FEIndex* face = (FS_FEIndex*)target_process_face_element->data;
        int vertex_xyz_pos = 0;
        for (vertex_xyz_pos; vertex_xyz_pos < 3; vertex_xyz_pos++) {
            FS_EAIndex point = face->p[vertex_xyz_pos];

            // Find if vertex cached
            rbtree_node* rbtree_search_result = NULL;
            rbtree_search_result = rbtree_search(vertex_lookup, point.cache_hash);
            if (rbtree_search_result) {
                list* rbnode_list = (list*)rbtree_search_result->data;
                FS_VCData* list_search_result = find_eaindex_from_list(rbnode_list, point.cache_string);
                if (list_search_result) {
                    // Cache success
                    vertex_index = list_search_result->index;
                    goto vertex_process_end;
                }
                goto rbtree_registered;
            }
            // Cache miss
            rbtree_insert(&vertex_lookup, point.cache_hash, (void*)create_list(), 0);
            rbtree_search_result = rbtree_search(vertex_lookup, point.cache_hash);

        rbtree_registered: {
            int vp_pointer = (point.pos - 1) * 3;
            int nm_pointer = (point.pos - 1) * 3;
            int uv_pointer = (point.uv - 1) * 2;
            list_append(list_vp_ordered, arr_vertex + vp_pointer, 0);
            if (obj_type[1] & 0b01) list_append(list_nm_ordered, arr_normal + nm_pointer, 0);
            if (obj_type[1] & 0b10) list_append(list_uv_ordered, arr_texcoord + uv_pointer, 0);
            vertex_index = list_vp_ordered->length - 1;

            list* rbnode_list = (list*)rbtree_search_result->data;
            FS_VCData* new_vcdata = (FS_VCData*)calloc(1, sizeof(FS_VCData));
            ALLOC_FAILCHECK(new_vcdata);
            new_vcdata->index = list_vp_ordered->length - 1;
            strcpy(new_vcdata->cache_string, point.cache_string);
            list_append(rbnode_list, new_vcdata, 0);
            }

    vertex_process_end:
        arr_element_buffer[arr_element_buffer_index++] = vertex_index;
        }

        target_process_face_element = target_process_face_element->next;
    }

    free_list(&list_vertex);
    free_list(&list_normal);
    free_list(&list_texcoord);
    free_list(&obj_index);

    // Now convert list_(vp|uv|nm)_ordered to array
    // arr_vb_(normal|texcoord) won't be NULL even though nmemb(or _Count) is zero.
    float* arr_vb_vertex = (float*)calloc(list_vp_ordered->length * 3, sizeof(float));
    float* arr_vb_normal = (float*)calloc(list_nm_ordered->length * 3, sizeof(float));
    float* arr_vb_texcoord = (float*)calloc(list_uv_ordered->length * 2, sizeof(float));
    ALLOC_FAILCHECK(arr_vb_vertex);
    ALLOC_FAILCHECK(arr_vb_normal);
    ALLOC_FAILCHECK(arr_vb_texcoord);

    int arr_vb_vtx_idx = 0;
    list_element* tmp_ordered_element = list_vp_ordered->head;
    while (tmp_ordered_element) {
        int vertex_xyz_pos;
        for (vertex_xyz_pos = 0; vertex_xyz_pos < 3; vertex_xyz_pos++) {
            // Ordered vertex position list to array
            arr_vb_vertex[arr_vb_vtx_idx * 3 + vertex_xyz_pos] = *(((float*)tmp_ordered_element->data) + vertex_xyz_pos);
        }
        arr_vb_vtx_idx++;
        tmp_ordered_element = tmp_ordered_element->next;
    }

    if (obj_type[1] & 0b01) {  // Normal
        arr_vb_vtx_idx = 0;
        tmp_ordered_element = list_nm_ordered->head;
        while (tmp_ordered_element) {
            int vertex_xyz_pos;
            for (vertex_xyz_pos = 0; vertex_xyz_pos < 3; vertex_xyz_pos++) {
                arr_vb_normal[arr_vb_vtx_idx * 3 + vertex_xyz_pos] = *(((float*)tmp_ordered_element->data) + vertex_xyz_pos);
            }
            arr_vb_vtx_idx++;
            tmp_ordered_element = tmp_ordered_element->next;
        }
    }
    if (obj_type[1] & 0b10) {  // Texture Coordinate
        arr_vb_vtx_idx = 0;
        tmp_ordered_element = list_uv_ordered->head;
        while (tmp_ordered_element) {
            int vertex_uv_pos;
            for (vertex_uv_pos = 0; vertex_uv_pos < 2; vertex_uv_pos++) {
                arr_vb_texcoord[arr_vb_vtx_idx * 2 + vertex_uv_pos] = *(((float*)tmp_ordered_element->data) + vertex_uv_pos);
            }
            arr_vb_vtx_idx++;
            tmp_ordered_element = tmp_ordered_element->next;
        }
    }
    
    container->vb_vertex = arr_vb_vertex;
    container->vb_vertex_size = list_vp_ordered->length * 3;
    container->vb_normal = arr_vb_normal;
    container->vb_normal_size = list_nm_ordered->length * 3;
    container->vb_texcoord = arr_vb_texcoord;
    container->vb_texcoord_size = list_uv_ordered->length * 2;

    container->element_buffer = arr_element_buffer;
    container->element_buffer_size = face_count * 3;

    container->collision_radius = max_radius;

    // MUST NOT FREE list_(vp|nm|uv)_ordered directly,
    // because those lists' data pointers are pointing middle of float heap!
    free(arr_vertex);
    free(arr_normal);
    free(arr_texcoord);
    list_element* tmp_for_free = list_vp_ordered->head;
    while (tmp_for_free) {
        tmp_for_free->data = NULL;
        tmp_for_free = tmp_for_free->next;
    }
    free_list(&list_vp_ordered);
    tmp_for_free = list_nm_ordered->head;
    while (tmp_for_free) {
        tmp_for_free->data = NULL;
        tmp_for_free = tmp_for_free->next;
    }
    free_list(&list_nm_ordered);
    tmp_for_free = list_uv_ordered->head;
    while (tmp_for_free) {
        tmp_for_free->data = NULL;
        tmp_for_free = tmp_for_free->next;
    }
    free_list(&list_uv_ordered);

    return 0;

error_after_list_creation:
    return 1;
}

node* FSnode_loadOBJ(node* target, const char* filename) {
    NULL_CHECK(target);
    fs_3d_data* target_data = (fs_3d_data*)target->data;
    NULL_CHECK(target_data);

    assert(target_data->shader_program && "Shader must be set to node while loading model!\n");
    assert(!target_data->vertex_array && "Vertex array already set on node!\n");

    FS_LoaderOBJ_DataContainer* container = (FS_LoaderOBJ_DataContainer*)calloc(1, sizeof(FS_LoaderOBJ_DataContainer));
    ALLOC_FAILCHECK(container);
    container->texture_list = create_list();
    loadOBJ_new(filename, container);

    target_data->collision_radius = container->collision_radius;
    target_data->texture_list = container->texture_list;

    glGenVertexArrays(1, &(target_data->vertex_array));
    glBindVertexArray(target_data->vertex_array);
    glUseProgram(target_data->shader_program);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);

    /* Initialize Geometry */
    // Set vertex position first
    GLint attrib_vert_loc = glGetAttribLocation(target_data->shader_program, "fs_Vertex");
    if (attrib_vert_loc != -1) {
        assert(!target_data->vertex_buffer_position && "Vertex buffer for position already set on node!\n");
        glGenBuffers(1, &(target_data->vertex_buffer_position));
        glBindBuffer(GL_ARRAY_BUFFER, target_data->vertex_buffer_position);
        glBufferData(GL_ARRAY_BUFFER, container->vb_vertex_size * sizeof(float), container->vb_vertex, GL_STATIC_DRAW);
        glVertexAttribPointer(attrib_vert_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(attrib_vert_loc);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    attrib_vert_loc = glGetAttribLocation(target_data->shader_program, "fs_Normal");
    if (attrib_vert_loc != -1 && container->vb_normal_size) {
        assert(!target_data->vertex_buffer_normal && "Vertex buffer for normal already set on node!\n");
        glGenBuffers(1, &(target_data->vertex_buffer_normal));
        glBindBuffer(GL_ARRAY_BUFFER, target_data->vertex_buffer_normal);
        glBufferData(GL_ARRAY_BUFFER, container->vb_normal_size * sizeof(float), container->vb_normal, GL_STATIC_DRAW);
        glVertexAttribPointer(attrib_vert_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(attrib_vert_loc);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    attrib_vert_loc = glGetAttribLocation(target_data->shader_program, "fs_MultiTexCoord0");
    if (attrib_vert_loc != -1 && container->vb_texcoord_size) {
        if (!target_data->vertex_buffer_texcoord_list) {
            target_data->vertex_buffer_texcoord_list = create_list();
        }
        assert(!target_data->vertex_buffer_texcoord_list->length && "Vertex buffer for texcoord already set on node!\n");
        if (!target_data->vertex_buffer_texcoord_list)
            target_data->vertex_buffer_texcoord_list = create_list();
        list_append(target_data->vertex_buffer_texcoord_list, calloc(1, sizeof(GLuint)), 0);

        list_element* target_texcoord_element = target_data->vertex_buffer_texcoord_list->head;

        glGenBuffers(1, &(target_texcoord_element->data));
        glBindBuffer(GL_ARRAY_BUFFER, target_texcoord_element->data);
        glBufferData(GL_ARRAY_BUFFER, container->vb_texcoord_size * sizeof(float), container->vb_texcoord, GL_STATIC_DRAW);
        glVertexAttribPointer(attrib_vert_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(attrib_vert_loc);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Populate element buffer
    glGenBuffers(1, &(target_data->element_buffer));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, target_data->element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, container->element_buffer_size * sizeof(int), container->element_buffer, GL_STATIC_DRAW);
    target_data->element_buffer_size = container->element_buffer_size;

    GLint uniform_model_mat_loc = glGetUniformLocation(target_data->shader_program, "fs_ModelMatrix");
    GLint uniform_view_mat_loc = glGetUniformLocation(target_data->shader_program, "fs_ViewMatrix");
    GLint uniform_proj_mat_loc = glGetUniformLocation(target_data->shader_program, "fs_ProjectionMatrix");

    glUniformMatrix4fv(uniform_model_mat_loc, 1, GL_TRUE, target_data->cumulative_model_mat->mat);
    glUniformMatrix4fv(uniform_view_mat_loc, 1, GL_TRUE, FS_ViewMatrix->mat);
    glUniformMatrix4fv(uniform_proj_mat_loc, 1, GL_TRUE, FS_ProjectionMatrix->mat);

    err = glGetError();
    if (err != GL_NO_ERROR) dprint("ERROR - glError 8: 0x%04X\n", err);

    free(container->vb_vertex);
    free(container->vb_normal);
    free(container->vb_texcoord);
    free(container->element_buffer);
    free(container);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return target;
}
