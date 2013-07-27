//
//  Created by Kevin Wojniak on 7/23/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#ifndef GLIMAGE_H
#define GLIMAGE_H

#include <stddef.h>
#if _WIN32
#include <windows.h>
#include <gl/gl.h>
#else
#include <OpenGL/OpenGL.h>
#endif
#include "GLRect.h"

class GLImage {
public:
    GLImage();
    
    void load(const void *buf, size_t bufSize);
    bool isLoaded() const;
    
    void draw(const GLRect& destRect, const GLRect& srcRect);
    void draw(const GLRect& destRect);
    void draw(int x, int y);
    
    int width() const;
    int height() const;

private:
	void loadTextureData_(void *texData);
#if _WIN32
    bool loadWin32Texture_(IStream *stream);
#endif
    
    GLuint texture_;
    int width_, height_;
};

#endif
