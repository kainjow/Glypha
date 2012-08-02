//
//  GLRenderer.h
//  Glypha
//
//  Created by Kevin Wojniak on 7/19/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "GLRect.h"

class GLRenderer {
public:
    GLRenderer();
    virtual ~GLRenderer();
    
    virtual void resize(int width, int height);
    virtual void clear();
    
    virtual void fillRect(const GLRect &rect);
    virtual void setFillColor(int red, int green, int blue);

    GLRect bounds();
    
private:
    GLRect bounds_;
    bool didPrepare_;
};

#endif
