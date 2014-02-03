//
//  Created by Kevin Wojniak on 7/23/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#include "GLImage.h"
#if _WIN32
#include <wincodec.h>
#include <shlwapi.h>
#elif __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#elif GLYPHA_HAVE_LIBPNG
#include <png.h>
#include "GLBufferReader.h"
#endif

GLImage::GLImage() :
    texture_(0), width_(0), height_(0)
{
}

#if GLYPHA_HAVE_LIBPNG
static void pngReader(png_structp png_ptr, png_bytep bytes, png_size_t size)
{
    GLBufferReader *reader = (GLBufferReader*)png_get_io_ptr(png_ptr);
    if (reader->read(bytes, size) != size) {
        memset(bytes, 0, size);
    }
}
#endif

void GLImage::load(const void *buf, size_t bufSize)
{
#if _WIN32
    IStream *stream = SHCreateMemStream((const BYTE*)buf, (UINT)bufSize);
    if (stream != NULL) {
        IWICImagingFactory *pFactory;
        if (CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pFactory) == S_OK) {
            IWICBitmapDecoder *pDecoder;
            if (pFactory->CreateDecoderFromStream(stream, &CLSID_WICPngDecoder, WICDecodeMetadataCacheOnDemand, &pDecoder) == S_OK) {
                IWICBitmapFrameDecode *frame;
                if (pDecoder->GetFrame(0, &frame) == S_OK) {
                    UINT w, h;
                    if (frame->GetSize(&w, &h) == S_OK) {
                        width_ = w;
                        height_ = h;
                    }
                    IWICFormatConverter *formatConverter;
                    if (pFactory->CreateFormatConverter(&formatConverter) == S_OK) {
                        if (formatConverter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom) == S_OK) {
                            unsigned char *pixels = new unsigned char[w * h * 4];
                            if (formatConverter->CopyPixels(0, w * 4, w * h * 4, pixels) == S_OK) {
                                loadTextureData_(pixels);
                            }
                            delete[] pixels;
                        }
                        formatConverter->Release();
                    }
                }
                pDecoder->Release();
            }
            pFactory->Release();
        }
        stream->Release();
    }
#elif __APPLE__
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (const UInt8*)buf, (CFIndex)bufSize, kCFAllocatorNull);
    if (data != NULL) {
        CGImageSourceRef imageSource = CGImageSourceCreateWithData(data, NULL);
        if (imageSource != NULL) {
            CGImageRef img = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
            if (img != NULL) {
                width_ = (int)CGImageGetWidth(img);
                height_ = (int)CGImageGetHeight(img);
                CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
                if (colorSpace != NULL) {
                    char *texData = (char*)calloc(width_ * 4 * height_, sizeof(char));
                    CGContextRef ctx = CGBitmapContextCreate(texData, width_, height_, 8, width_*4, colorSpace, kCGBitmapByteOrder32Host| kCGImageAlphaPremultipliedFirst);
                    if (ctx != NULL) {
                        CGContextDrawImage(ctx, CGRectMake(0.0, 0.0, width_, height_), img);
                        CGContextRelease(ctx);
                        loadTextureData_(texData);
                    }
                    free(texData);
                    CGColorSpaceRelease(colorSpace);
                }
                CGImageRelease(img);
            }
            CFRelease(imageSource);
        }
        CFRelease(data);
    }
#elif GLYPHA_HAVE_LIBPNG
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
#endif
}

bool GLImage::isLoaded() const
{
    return texture_ != 0;
}

void GLImage::loadTextureData_(const void *texData, bool hasAlpha)
{
	// set pixel modes
	glPixelStorei(GL_UNPACK_ROW_LENGTH, width_);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	// generate new texture name and bind it
    glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	
	// GL_REPLACE prevents colors from seeping into a texture
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	// GL_NEAREST affects drawing the texture at different sizes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	// set texture data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, hasAlpha ? GL_BGRA_EXT : GL_BGR_EXT, GL_UNSIGNED_BYTE, texData);
}

void GLImage::draw(const GLRect& destRect, const GLRect& srcRect)
{
	// set this texture as current
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_);
	
	// enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
    // calculate texture coordiantes.
	float srcMinY = (float)srcRect.top() / height_;
	float srcMaxY = (float)srcRect.bottom() / height_;
	float srcMinX = (float)srcRect.left() / width_;
	float srcMaxX = (float)srcRect.right() / width_;
    
	// draw the texture
	glBegin(GL_QUADS);
	glTexCoord2f(srcMinX, srcMinY);
    glVertex2i(destRect.left(), destRect.top());
	glTexCoord2f(srcMinX, srcMaxY);
    glVertex2i(destRect.left(), destRect.bottom());
	glTexCoord2f(srcMaxX, srcMaxY);
    glVertex2i(destRect.right(), destRect.bottom());
	glTexCoord2f(srcMaxX, srcMinY);
    glVertex2i(destRect.right(), destRect.top());
	glEnd();
	
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void GLImage::draw(const GLRect& destRect)
{
    draw(destRect, GLRect(0, 0, width_, height_));
}

void GLImage::draw(int x, int y)
{
    draw(GLRect(x, y, width_, height_), GLRect(0, 0, width_, height_));
}

int GLImage::width() const
{
    return width_;
}

int GLImage::height() const
{
    return height_;
}
