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
#include <OpenGL/gl.h>
#endif

GLImage::GLImage() :
    texture_(0), width_(0), height_(0)
{
}

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
    CGContextRef ctx;
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (const UInt8*)buf, (CFIndex)bufSize, kCFAllocatorNull);
    if (data != NULL) {
        CGImageSourceRef imageSource = CGImageSourceCreateWithData(data, NULL);
        if (imageSource != NULL) {
            CGImageRef img = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
            if (img != NULL) {
                width_ = (int)CGImageGetWidth(img);
                height_ = (int)CGImageGetHeight(img);
                char *texData = (char*)calloc(width_ * 4 * height_, sizeof(char));
                CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
                if (colorSpace != NULL) {
                    ctx = CGBitmapContextCreate(texData, width_, height_, 8, width_*4, colorSpace, kCGBitmapByteOrder32Host| kCGImageAlphaPremultipliedFirst);
                    if (ctx != NULL) {
                        CGContextDrawImage(ctx, CGRectMake(0.0, 0.0, width_, height_), img);
                        loadTextureData_(texData);
                        CGContextRelease(ctx);
                    }
                    CGColorSpaceRelease(colorSpace);
                }
                free(texData);
                CGImageRelease(img);
            }
            CFRelease(imageSource);
        }
        CFRelease(data);
    }
#endif
}

bool GLImage::isLoaded() const
{
    return texture_ != 0;
}

void GLImage::loadTextureData_(void *texData)
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, texData);
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
