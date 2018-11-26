#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "GLRect.h"
#include "GLPoint.h"
#include "GLImage.h"

namespace GL {

class Renderer {
public:
    virtual ~Renderer() {}
    
    virtual void resize(int width, int height) = 0;
    virtual void clear() = 0;
    
    virtual void fillRect(const Rect& rect) = 0;
    virtual void setFillColor(float red, float green, float blue) = 0;

    virtual void beginLines(float lineWidth, bool smooth = true) = 0;
    virtual void endLines() = 0;
    virtual void moveTo(int h, int v) = 0;
    virtual void lineTo(int h, int v) = 0;
    
    virtual Rect bounds() = 0;
    
    virtual Image makeImage(const unsigned char *buf, size_t bufSize) = 0;
    
/*private:
    Rect bounds_;
    bool didPrepare_;
    Point lineStart_;*/
};

}

#endif
