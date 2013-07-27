//
//  GLImage.cpp
//  Glypha
//
//  Created by Kevin Wojniak on 7/23/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#include "GLImage.h"
#if _WIN32
#include <gdiplus.h>
#include <wincodec.h>
#include <shlwapi.h>
#include <strsafe.h>
#elif __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#include <OpenGL/gl.h>
#endif

#if _WIN32
// loadWin32Texture_() is based on code from the "OpenGL GDI+ Demo" at
// http://www.dhpoware.com/demos/glGdiplus.html
//-----------------------------------------------------------------------------
// Copyright (c) 2009 dhpoware. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------
bool GLImage::loadWin32Texture_(IStream *stream)
{
    static bool didLoadGdi = false;
    if (didLoadGdi == false) {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) == Gdiplus::Ok) {
            didLoadGdi = true;
        } else {
            return false;
        }
    }

    Gdiplus::Bitmap *pBitmap = Gdiplus::Bitmap::FromStream(stream, TRUE);
    if (pBitmap == NULL) {
        return false;
    }

    // GDI+ pads each scanline of the loaded bitmap image to 4-byte memory
    // boundaries. Fortunately OpenGL also aligns bitmap images to 4-byte
    // memory boundaries by default.
    width_ = pBitmap->GetWidth();
    height_ = pBitmap->GetHeight();
    int pitch = ((width_ * 32 + 31) & ~31) >> 3;

    Gdiplus::BitmapData data;
    Gdiplus::Rect rect(0, 0, width_, height_);
    // Convert to 32-bit BGRA pixel format and fetch the pixel data.
    if (pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data) != Gdiplus::Ok) {
        return false;
    }
    unsigned char *pixels = (unsigned char*)malloc(pitch * height_);
    unsigned char *pSrcPixels = static_cast<unsigned char *>(data.Scan0);
    for (int i = 0; i < height_; ++i) {
        memcpy(&pixels[i * pitch], &pSrcPixels[i * data.Stride], pitch);
    }
    (void)pBitmap->UnlockBits(&data);
    loadTextureData_(pixels);
    free(pixels);

    return true;
}
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
#if 1
        int ret = loadWin32Texture_(stream);
#else
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
                    WICPixelFormatGUID px;
                    if (frame->GetPixelFormat(&px) == S_OK) {
                        WCHAR buf[100];
                        StringCchPrintf(buf, sizeof(buf)/sizeof(buf[0]), L"px=%u\n", px);
                        OutputDebugString(buf);
                    }

                    IWICFormatConverter *formatConverter;
                    if (pFactory->CreateFormatConverter(&formatConverter) == S_OK) {
                        formatConverter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
                    }
                }
                pDecoder->Release();
            }
            pFactory->Release();
        }
#endif
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

void GLImage::draw(GLRect destRect, GLRect srcRect)
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

void GLImage::draw(GLRect destRect)
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
