#ifndef GLIMAGE_H
#define GLIMAGE_H

#include "GLOpenGL.h"
#include "GLRect.h"
#include "GLPoint.h"
#include <stddef.h>
#include <functional>

namespace GL {

class Image {
public:
    Image();
    
    void load(const unsigned char *buf, size_t bufSize);
    bool isLoaded() const;
    void setLoaded(bool loaded);

    void draw(const Point *dest, size_t numDest, const Point *src, size_t numSrc) const;
    void draw(const Rect& destRect, const Rect& srcRect) const;
    void draw(const Rect& destRect) const;
    void draw(int x, int y) const;
    
    using DrawCallback = std::function<void(const Rect& destRect, const Rect& srcRect)>;
    void setDrawCallback(const DrawCallback& callback);
    
    int width() const;
    int height() const;
    
    void setAllowColorBlending(bool colorBlending);

private:
#if GLYPHA_USE_OPENGL
	void loadTextureData_(const void *texData, bool hasAlpha = true);
	void loadTextureData_(const void *texData, GLenum format, bool hasAlpha = true);
#endif
    
    GLuint texture_;
    int width_, height_;
    bool alpha_;
    bool colorBlending_;
    DrawCallback drawCallback_;
};
    
}

#endif
