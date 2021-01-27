#include "3d_vox.h"

FS_LoaderVox_DefaultPalette[256] = {
   0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
   0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
   0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
   0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
   0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
   0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
   0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
   0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
   0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
   0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
   0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
   0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
   0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
   0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
   0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
   0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
};

int loadVOX(char* filename) {
    FILE* file_pointer = fopen(filename, "r");
    if (file_pointer == NULL) {
        dprint("VOX_LOADER > VOX file load Failed.\n");
        return 1;
    }

    // Read file informations, such as magic and version
    char file_magic[5] = { 0 };
    int file_version = 0;
    if (fread(file_magic, sizeof(char), 4, file_pointer) != 4 || strcmp(file_magic, "VOX ")) {
        dprint("VOX_LOADER > This is not a valid VOX file.\n");
        return 1;
    }

    if (!fread(&file_version, sizeof(int), 1, file_pointer)) {
        dprint("VOX_LOADER > Cannot read version from VOX file.\n");
        return 1;
    }
    dprint("VOX file version: %u\n", file_version);

    // Now, process chunks
    list* material_list = create_list();
    list* palette_list = create_list();
    list* model_list = create_list();

    unsigned int model_num = 0;
    FS_LoaderVox_Model* current_model = NULL;

    // There are various types of chunks, we need to identify and process this
    while (1) {
        // Parse chunk type id and size, and get size of children chunks
        char chunk_id[5] = { 0 };
        int chunk_size = 0;
        int child_chunk_size = 0;
        if (fread(chunk_id, sizeof(char), 4, file_pointer) != 4) {
            if (!model_list->length) {
                dprint("VOX_LOADER > EOF while parsing VOX file (Chunk ID)\n");
                goto parse_failed;
            }
            // Model file ended
            break;
        }
        if (!fread(&chunk_size, sizeof(int), 1, file_pointer)) {
            dprint("VOX_LOADER > EOF while parsing VOX file (Chunk size)\n");
            goto parse_failed;
        }
        if (!fread(&child_chunk_size, sizeof(int), 1, file_pointer)) {
            dprint("VOX_LOADER > EOF while parsing VOX file (Child chunk size)\n");
            goto parse_failed;
        }

        // Now, process chunks
        if (!strcmp(chunk_id, "MAIN")) {
            // Root chunk and parent chunk of all the other chunks
        }
        else if (!strcmp(chunk_id, "PACK")) {
            int parsed_num_models = 0;
            if (!fread(&parsed_num_models, sizeof(int), 1, file_pointer)) {
                dprint("VOX_LOADER > EOF while parsing VOX file (PACK chunk numModels)\n");
                goto parse_failed;
            }

            if (!model_num) {
                model_num = parsed_num_models;
                continue;
            }
            else {
                dprint("VOX_LOADER > Exception raised while parsing VOX file (multiple PACK exists)\n");
            }
        }
        else if (!strcmp(chunk_id, "SIZE")) {
            int size_x, size_y, size_z;
            if (fread(&size_x, sizeof(int), 1, file_pointer) != 1 ||
                fread(&size_y, sizeof(int), 1, file_pointer) != 1 ||
                fread(&size_z, sizeof(int), 1, file_pointer) != 1) {
                dprint("VOX_LOADER > EOF while parsing VOX file (SIZE chunk)\n");
                goto parse_failed;
            }
            if (!(0 < size_x) || !(0 < size_y) || !(0 < size_z)) {
                dprint("VOX_LOADER > Exception raised while parsing VOX file (X/Y/Z must be bigger than 0)\n");
                goto parse_failed;
            }

            current_model = (FS_LoaderVox_Model*)calloc(1, sizeof(FS_LoaderVox_Model));
            current_model->size_x = size_x;
            current_model->size_y = size_y;
            current_model->size_z = size_z;
            current_model->voxels = create_list();
            list_append(model_list, current_model, strdup("FS_LoaderVox_Model"));
        }
        else if (!strcmp(chunk_id, "XYZI")) {
            if (current_model == NULL) {
                dprint("VOX_LOADER > Exception raised while parsing VOX file (XYZI showed before SIZE chunk)\n");
                goto parse_failed;
            }

            int parsed_num_voxels = 0;
            if (!fread(&parsed_num_voxels, sizeof(int), 1, file_pointer)) {
                dprint("VOX_LOADER > EOF while parsing VOX file (XYZI chunk)\n");
                goto parse_failed;
            }
            if (!parsed_num_voxels) {
                dprint("VOX_LOADER > Model must contain at least one voxel");
                goto parse_failed;
            }

            printf("%d\n", parsed_num_voxels);
            int a = 0;
            for (a; a < parsed_num_voxels; a++) {
                char x = 0, y = 0, z = 0, color_index = 0;
                if (fread(&x, sizeof(char), 1, file_pointer) != 1 ||
                    fread(&y, sizeof(char), 1, file_pointer) != 1 ||
                    fread(&z, sizeof(char), 1, file_pointer) != 1 ||
                    fread(&color_index, sizeof(char), 1, file_pointer) != 1) {
                    printf("%d, %d, %d [%d]\n", x, y, z, color_index);
                    dprint("VOX_LOADER > EOF while parsing VOX file (XYZI child chunk)\n");
                    goto parse_failed;
                }
                if (0 > color_index) {
                    dprint("VOX_LOADER > Exception raised while parsing VOX file (XYZI color index not positive)\n");
                    goto parse_failed;
                }
                FS_LoaderVox_Voxel* new_voxel = (FS_LoaderVox_Voxel*)calloc(1, sizeof(FS_LoaderVox_Voxel));
                new_voxel->x = x;
                new_voxel->y = y;
                new_voxel->z = z;
                new_voxel->color_index = color_index;
                list_append(current_model->voxels, new_voxel, strdup("FS_LoaderVox_Voxel"));
            }
        }
        //else if (!strcmp(chunk_id, "MATT")) {
        //    // Material V1(Deprecated) chunk
        //    fseek(file_pointer, chunk_size, SEEK_CUR);
        //}
        //else if (!strcmp(chunk_id, "MATL")) {
        //    // Material V2 chunk
        //    fseek(file_pointer, chunk_size, SEEK_CUR);
        //}
        else if (!strcmp(chunk_id, "RGBA")) {
            // Palette chunk
            fseek(file_pointer, chunk_size, SEEK_CUR);
        }
        // Belows are not documented yet
        //else if (!strcmp(chunk_id, "nTRN")) {
        //    fseek(file_pointer, chunk_size, SEEK_CUR);
        //}
        //else if (!strcmp(chunk_id, "nGRP")) {
        //    fseek(file_pointer, chunk_size, SEEK_CUR);
        //}
        //else if (!strcmp(chunk_id, "nSHP")) {
        //    fseek(file_pointer, chunk_size, SEEK_CUR);
        //}
        //else if (!strcmp(chunk_id, "LAYR")) {
        //    fseek(file_pointer, chunk_size, SEEK_CUR);
        //}
        //else if (!strcmp(chunk_id, "rOBJ")) {
        //    fseek(file_pointer, chunk_size, SEEK_CUR);
        //}
        //else if (!strcmp(chunk_id, "IMAP")) {
        //    fseek(file_pointer, chunk_size, SEEK_CUR);
        //}
        //else if (!strcmp(chunk_id, "NOTE")) {
        //    fseek(file_pointer, chunk_size, SEEK_CUR);
        //}

        else {
            // Handle unidentified type of chunks
            dprint("VOX_LOADER > Unexpected type of chunk is included in VOX file.\n");
            fseek(file_pointer, chunk_size, SEEK_CUR);
        }
    }

    if (model_num && model_num != model_list->length) {
        dprint("VOX_LOADER > The number of models are not matching as PACK chunk described in VOX file.\n");
        goto parse_failed;
    }


    list_element* cur_element = model_list->head;
    while (cur_element) {
        FS_LoaderVox_Model* model = (FS_LoaderVox_Model*)cur_element->data;
        printf("%d x %d x %d model loaded(numVoxels = %d)\n",
               model->size_x, model->size_y, model->size_z, model->voxels->length);
        cur_element = cur_element->next;
    }

    return 0;

parse_failed:
    free_list(&material_list);
    free_list(&palette_list);
    free_list(&model_list);
    return 1;
}


node* FSnode_loadVOX(node* target, const char* filename) {

}
