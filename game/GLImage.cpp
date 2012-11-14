//
//  GLImage.cpp
//  Glypha
//
//  Created by Kevin Wojniak on 7/23/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#include "GLImage.h"
#if __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#include <OpenGL/gl.h>
#endif

GLImage::GLImage(void *buf, size_t bufSize) :
    texture_(0), width_(0), height_(0)
{
#if __APPLE__
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV, texData);
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
    glVertex2f(destRect.left(), destRect.bottom());
	glTexCoord2f(srcMaxX, srcMaxY);
    glVertex2f(destRect.right(), destRect.bottom());
	glTexCoord2f(srcMaxX, srcMinY);
    glVertex2f(destRect.right(), destRect.top());
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
