//
//  Created by Kevin Wojniak on 2014/2/2.
//  Copyright (c) 2014 Kevin Wojniak. All rights reserved.
//

#include "GLImage.h"
#include <ApplicationServices/ApplicationServices.h>

void GLImage::load(const unsigned char *buf, size_t bufSize)
{
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
                    char *texData = (char*)calloc(width_ * height_ * 4, sizeof(char));
                    CGContextRef ctx = CGBitmapContextCreate(texData, width_, height_, 8, width_ * 4, colorSpace, kCGBitmapByteOrder32Host | kCGImageAlphaPremultipliedFirst);
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
}
