#include "GLImage.h"
#include <ApplicationServices/ApplicationServices.h>
#include <vector>

void GL::Image::load(const unsigned char *buf, size_t bufSize)
{
    CFDataRef data = CFDataCreateWithBytesNoCopy(
        kCFAllocatorDefault,
        reinterpret_cast<const UInt8*>(buf),
        static_cast<CFIndex>(bufSize),
        kCFAllocatorNull
    );
    if (data != NULL) {
        CGImageSourceRef imageSource = CGImageSourceCreateWithData(data, NULL);
        if (imageSource != NULL) {
            CGImageRef img = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
            if (img != NULL) {
                width_ = static_cast<int>(CGImageGetWidth(img));
                height_ = static_cast<int>(CGImageGetHeight(img));
                CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
                if (colorSpace != NULL) {
                    std::vector<char> texData(static_cast<std::vector<char>::size_type>(width_ * height_ * 4));
                    CGContextRef ctx = CGBitmapContextCreate(
                        &texData[0],
                        static_cast<size_t>(width_),
                        static_cast<size_t>(height_),
                        8,
                        static_cast<size_t>(width_ * 4),
                        colorSpace,
                        kCGBitmapByteOrder32Host | kCGImageAlphaPremultipliedFirst
                    );
                    if (ctx != NULL) {
                        CGContextDrawImage(ctx, CGRectMake(0.0, 0.0, width_, height_), img);
                        CGContextRelease(ctx);
                        loadTextureData_(&texData[0]);
                    }
                    CGColorSpaceRelease(colorSpace);
                }
                CGImageRelease(img);
            }
            CFRelease(imageSource);
        }
        CFRelease(data);
    }
}
