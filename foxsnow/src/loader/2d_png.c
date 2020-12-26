#include "2d_png.h"

TextureInfo* loadPNG(const char* path, GLint filter_min, GLint filter_mag, GLint wrap_s, GLint wrap_t) {
    FILE* f;
    int bit_depth, color_type, row_bytes, i;
    png_infop info_ptr = NULL, end_info = NULL;
    png_uint_32 t_width, t_height;
    png_byte header[PNG_BYTES_TO_CHECK] = { 0 }, *image_data = NULL;
    png_bytepp row_pointers = NULL;
    png_structp png_ptr = NULL;
    GLuint texture = 0;
    int alpha;

    TextureInfo* result_texture = (TextureInfo*)calloc(1, sizeof(TextureInfo));
    ALLOC_FAILCHECK(result_texture);
    // Set default texture stat
    result_texture->wrap_s = wrap_s;
    result_texture->wrap_t = wrap_t;

    result_texture->filter_min = filter_min;
    result_texture->filter_mag = filter_mag;

    if (!(f = fopen(path, "rb"))) {
        dprint("Exception raised: Failed to open file.\n");
        return NULL;
    }
    fread(header, 1, PNG_BYTES_TO_CHECK, f);
    if (png_sig_cmp(header, 0, PNG_BYTES_TO_CHECK)) {
        dprint("Exception raised: Not a PNG file.\n");
        fclose(f);
        return NULL;
    }
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        dprint("Exception raised: Failed to create read struct.\n");
        fclose(f);
        return NULL;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        dprint("Exception raised: Failed to create start info struct.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(f);
        return NULL;
    }
    end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        dprint("Exception raised: Failed to create end info struct.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(f);
        return NULL;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        dprint("Exception raised: setjmp set.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(f);
        return NULL;
    }
    png_init_io(png_ptr, f);
    png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &t_width, &t_height, &bit_depth, &color_type, NULL, NULL, NULL);

    png_read_update_info(png_ptr, info_ptr);
    row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    image_data = (png_bytep)malloc(row_bytes * t_height * sizeof(png_byte));
    if (!image_data) {
        dprint("Exception raised: image_data is NULL.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(f);
        return NULL;
    }
    row_pointers = (png_bytepp)malloc(t_height * sizeof(png_bytep));
    if (!row_pointers) {
        dprint("Exception raised: row_pointers is NULL.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(f);
        return NULL;
    }
    for (i = 0; i < t_height; ++i) {
        row_pointers[t_height - 1 - i] = image_data + i * row_bytes;
    }
    png_read_image(png_ptr, row_pointers);
    switch (png_get_color_type(png_ptr, info_ptr)) {
    case PNG_COLOR_TYPE_RGBA:
        alpha = GL_RGBA;
        break;
    case PNG_COLOR_TYPE_RGB:
        alpha = GL_RGB;
        break;
    default:
        dprint("Color type %d not supported!\n", png_get_color_type(png_ptr, info_ptr));
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return NULL;
    }
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t_width, t_height, 0, alpha, GL_UNSIGNED_BYTE, (GLvoid*)image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    glBindTexture(GL_TEXTURE_2D, 0);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(f);

    result_texture->width = t_width;
    result_texture->height = t_height;
    result_texture->texture_id = texture;
    result_texture->i_format = GL_RGBA;
    result_texture->format = alpha;
    result_texture->type = GL_UNSIGNED_BYTE;
    result_texture->attachment = 0;

    return result_texture;
}
