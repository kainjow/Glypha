#include "GLImage.h"

GLImage::GLImage()
    : texture_(0)
    , width_(0)
    , height_(0)
    , alpha_(false)
{
}

bool GLImage::isLoaded() const
{
    return texture_ != 0;
}

void GLImage::loadTextureData_(const void *texData, bool hasAlpha)
{
    alpha_ = hasAlpha;
    
	// set pixel modes
	glPixelStorei(GL_UNPACK_ROW_LENGTH, width_);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	// generate new texture name and bind it
    glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	
	// GL_REPLACE prevents colors from seeping into a texture
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	// GL_NEAREST affects drawing the texture at different sizes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	// set texture data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, hasAlpha ? GL_BGRA_EXT : GL_BGR_EXT, GL_UNSIGNED_BYTE, texData);
}


void GLImage::draw(const GLPoint *dest, size_t numDest, const GLPoint *src, size_t numSrc) const
{
    if (numDest != numSrc || numDest < 3) {
        // bug
        return;
    }
    
	// set this texture as current
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_);
	
    if (alpha_) {
        // enable alpha blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
	
	// draw the texture
	glBegin(numDest == 4 ? GL_QUADS : GL_POLYGON);
    for (size_t i = 0; i < numDest; ++i) {
        const GLPoint destPt = dest[i];
        const GLPoint srcPt = src[i];
        glTexCoord2f((float)srcPt.h / width_, (float)srcPt.v / height_);
        glVertex2i(destPt.h, destPt.v);
    }
	glEnd();
	
    if (alpha_) {
        glDisable(GL_BLEND);
    }
	glDisable(GL_TEXTURE_2D);
}

void GLImage::draw(const GLRect& destRect, const GLRect& srcRect) const
{
    GLPoint dest[4];
    GLPoint src[4];
    dest[0] = GLPoint(destRect.left, destRect.top);
    dest[1] = GLPoint(destRect.left, destRect.bottom);
    dest[2] = GLPoint(destRect.right, destRect.bottom);
    dest[3] = GLPoint(destRect.right, destRect.top);
    src[0] = GLPoint(srcRect.left, srcRect.top);
    src[1] = GLPoint(srcRect.left, srcRect.bottom);
    src[2] = GLPoint(srcRect.right, srcRect.bottom);
    src[3] = GLPoint(srcRect.right, srcRect.top);
    draw(dest, sizeof(dest) / sizeof(dest[0]), src, sizeof(src) / sizeof(src[0]));
}

void GLImage::draw(const GLRect& destRect) const
{
    draw(destRect, GLRect(0, 0, width_, height_));
}

void GLImage::draw(int x, int y) const
{
    draw(GLRect(x, y, width_, height_), GLRect(0, 0, width_, height_));
}

int GLImage::width() const
{
    return width_;
}

int GLImage::height() const
{
    return height_;
}
