#ifndef GLIMAGE_H
#define GLIMAGE_H

#include "GLRenderer.h"
#include "GLRect.h"
#include <stddef.h>

class GLImage {
public:
    GLImage();
    
    void load(const unsigned char *buf, size_t bufSize);
    bool isLoaded() const;
    
    void draw(const GLPoint *dest, size_t numDest, const GLPoint *src, size_t numSrc);
    void draw(const GLRect& destRect, const GLRect& srcRect);
    void draw(const GLRect& destRect);
    void draw(int x, int y);
    
    int width() const;
    int height() const;

private:
	void loadTextureData_(const void *texData, bool hasAlpha = true);
    
    GLuint texture_;
    int width_, height_;
    bool alpha_;
};

#endif
