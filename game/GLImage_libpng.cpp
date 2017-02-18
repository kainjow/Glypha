#include "GLImage.h"
#include "GLBufferReader.h"
#include <png.h>
#include <cstring>

namespace {
void pngReader(png_structp png_ptr, png_bytep bytes, png_size_t size)
{
    GLBufferReader *reader = (GLBufferReader*)png_get_io_ptr(png_ptr);
    if (reader->read(bytes, size) != size) {
        memset(bytes, 0, size);
    }
}
}

void GL::Image::load(const unsigned char *buf, size_t bufSize)
{
    GLBufferReader reader((const uint8_t*)buf, bufSize);
    png_byte sig[8];
    if (reader.read(sig, sizeof(sig)) == sizeof(sig)) {
        if (png_sig_cmp(sig, 0, sizeof(sig)) == 0) {
            png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            png_infop info_ptr = png_create_info_struct(png_ptr);
            png_infop end_info = png_create_info_struct(png_ptr);
            if (setjmp(png_jmpbuf(png_ptr))) {
                png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            }
            png_set_sig_bytes(png_ptr, sizeof(sig));
            png_set_read_fn(png_ptr, &reader, pngReader);
            png_read_info(png_ptr, info_ptr);
            
            // color type should be PNG_COLOR_TYPE_PALETTE
            width_ = png_get_image_width(png_ptr, info_ptr);
            height_ = png_get_image_height(png_ptr, info_ptr);
            
            png_set_expand(png_ptr);
            png_set_bgr(png_ptr); // for BGR for OpenGL texture
 
            png_read_update_info(png_ptr, info_ptr);
            png_byte color_type = png_get_color_type(png_ptr, info_ptr);
            
            png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
            png_byte *image_data = new png_byte[rowbytes * height_];
            png_bytep row_pointers[height_];
            for (int y = 0; y < height_; ++y) {
                row_pointers[y] = image_data + (y * rowbytes);
            }
            png_read_image(png_ptr, row_pointers);
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            loadTextureData_(image_data, color_type == PNG_COLOR_TYPE_RGB_ALPHA);
            delete[] image_data;
        }
    }
}
