#ifndef GLIMAGE_H
#define GLIMAGE_H

#include "GLRenderer.h"
#include "GLRect.h"
#include <stddef.h>

namespace GL {

class Image {
public:
    Image();
    
    void load(const unsigned char *buf, size_t bufSize);
    bool isLoaded() const;
    
    void draw(const Point *dest, size_t numDest, const Point *src, size_t numSrc) const;
    void draw(const Rect& destRect, const Rect& srcRect) const;
    void draw(const Rect& destRect) const;
    void draw(int x, int y) const;
    
    int width() const;
    int height() const;
    
    void setAllowColorBlending(bool colorBlending);

private:
	void loadTextureData_(const void *texData, bool hasAlpha = true);
	void loadTextureData_(const void *texData, GLenum format, bool hasAlpha = true);
    
    GLuint texture_;
    int width_, height_;
    bool alpha_;
    bool colorBlending_;
};
    
}

#endif
