//
//  GLImage.cpp
//  Glypha
//
//  Created by Kevin Wojniak on 7/23/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#include "GLImage.h"
#if _WIN32
#include <gl/glu.h>
#include <gdiplus.h>
#include <wincodec.h>
#include <shlwapi.h>
#include <strsafe.h>
#elif __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#include <OpenGL/gl.h>
#endif

#if _WIN32
static bool LoadTexture(IStream *stream, GLuint *g_texture, int *g_textureWidth, int *g_textureHeight)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    Gdiplus::Bitmap *pBitmap;

    pBitmap = Gdiplus::Bitmap::FromStream(stream, TRUE);
    if (pBitmap == NULL) {
        return false;
    }

    // GDI+ pads each scanline of the loaded bitmap image to 4-byte memory
    // boundaries. Fortunately OpenGL also aligns bitmap images to 4-byte
    // memory boundaries by default.
    int width = pBitmap->GetWidth();
    int height = pBitmap->GetHeight();
    int pitch = ((width * 32 + 31) & ~31) >> 3;

    unsigned char *pixels = (unsigned char*)malloc(pitch * height);
    //std::vector<unsigned char> pixels(pitch * height);
    Gdiplus::BitmapData data;
    Gdiplus::Rect rect(0, 0, width, height);

    // Convert to 32-bit BGRA pixel format and fetch the pixel data.
    if (pBitmap->LockBits(NULL/*&rect*/, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data) != Gdiplus::Ok) {
        return false;
    }
    unsigned char *pSrcPixels = static_cast<unsigned char *>(data.Scan0);
    for (int i = 0; i < height; ++i) {
        memcpy(&pixels[i * pitch], &pSrcPixels[i * data.Stride], pitch);
    }
    pBitmap->UnlockBits(&data);

    // Create an OpenGL texture object to store the loaded bitmap image.
    glGenTextures(1, g_texture);
    glBindTexture(GL_TEXTURE_2D, *g_texture);

    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, &pixels[0]);

    *g_textureWidth = width;
    *g_textureHeight = height;

    return 0;
}
#endif

GLImage::GLImage(void *buf, size_t bufSize) :
    texture_(0), width_(0), height_(0)
{
#if _WIN32
    IStream *stream = SHCreateMemStream((const BYTE*)buf, (UINT)bufSize);
    if (stream != NULL) {
#if 1
        int ret = LoadTexture(stream, &texture_, &width_, &height_);
        WCHAR buf[100];
        StringCchPrintf(buf, sizeof(buf)/sizeof(buf[0]), L"bufSize=%u, ret=%d, width=%u, height=%u\n", bufSize, ret, width_, height_);
        OutputDebugString(buf);
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
    CFDataRef data = NULL;
    CGImageSourceRef imageSource = NULL;
    CGImageRef img = NULL;
    void *texData;
    CGColorSpaceRef colorSpace;
    CGContextRef ctx;
    
    data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (const UInt8*)buf, (CFIndex)bufSize, kCFAllocatorNull);
    imageSource = CGImageSourceCreateWithData(data, NULL);
    img = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
	width_ = (int)CGImageGetWidth(img);
	height_ = (int)CGImageGetHeight(img);
	texData = calloc(width_ * 4, height_);
	colorSpace = CGColorSpaceCreateDeviceRGB();
	ctx = CGBitmapContextCreate(texData, width_, height_, 8, width_*4, colorSpace, kCGBitmapByteOrder32Host| kCGImageAlphaPremultipliedFirst);
	CGContextDrawImage(ctx, CGRectMake(0.0, 0.0, width_, height_), img);
	
	loadTextureData_(texData);

	CGContextRelease(ctx);
    CGColorSpaceRelease(colorSpace);
    free(texData);
	CGImageRelease(img);
#endif
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
#if _WIN32
    GLenum type = GL_UNSIGNED_BYTE;
#else
    GLenum type = GL_UNSIGNED_INT_8_8_8_8_REV;
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_BGRA_EXT, type, texData);
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

int GLImage::width()
{
    return width_;
}

int GLImage::height()
{
    return height_;
}
