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
    if (data != nullptr) {
        CGImageSourceRef imageSource = CGImageSourceCreateWithData(data, nullptr);
        if (imageSource != nullptr) {
            CGImageRef img = CGImageSourceCreateImageAtIndex(imageSource, 0, nullptr);
            if (img != nullptr) {
                width_ = static_cast<int>(CGImageGetWidth(img));
                height_ = static_cast<int>(CGImageGetHeight(img));
                CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
                if (colorSpace != nullptr) {
                    std::vector<char> texData(static_cast<std::vector<char>::size_type>(width_ * height_ * 4));
                    CGContextRef ctx = CGBitmapContextCreate(
                        texData.data(),
                        static_cast<size_t>(width_),
                        static_cast<size_t>(height_),
                        8,
                        static_cast<size_t>(width_ * 4),
                        colorSpace,
                        kCGBitmapByteOrder32Host | kCGImageAlphaPremultipliedFirst
                    );
                    if (ctx != nullptr) {
                        CGContextDrawImage(ctx, CGRectMake(0.0, 0.0, width_, height_), img);
                        CGContextRelease(ctx);
                        loadTextureData_(texData.data());
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
