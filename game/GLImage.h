//
//  Created by Kevin Wojniak on 7/23/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#ifndef GLIMAGE_H
#define GLIMAGE_H

#include "GLRenderer.h"
#include "GLRect.h"
#include <stddef.h>

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
	void loadTextureData_(const void *texData, bool hasAlpha = true);
#if _WIN32
    bool loadWin32Texture_(IStream *stream);
#endif
    
    GLuint texture_;
    int width_, height_;
};

#endif
