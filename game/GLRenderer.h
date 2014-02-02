//
//  Created by Kevin Wojniak on 7/19/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "GLRect.h"
#include "GLPoint.h"

#if __APPLE__
#include <OpenGL/gl.h>
#elif _WIN32
#include <windows.h>
#include <gl/gl.h>
#elif __HAIKU__
#include <GL/gl.h>
#elif GLYPHA_QT
#include <QtOpenGL>
#endif

class GLRenderer {
public:
    GLRenderer();
    virtual ~GLRenderer();
    
    void resize(int width, int height);
    void clear();
    
    void fillRect(const GLRect& rect);
    void setFillColor(int red, int green, int blue);

    void beginLines(float lineWidth);
    void endLines();
    void moveTo(int h, int v);
    void lineTo(int h, int v);
    void drawLine(int h1, int v1, int h2, int v2);
    
    GLRect bounds();
    
private:
    GLRect bounds_;
    bool didPrepare_;
    GLPoint lineStart_;
};

#endif
