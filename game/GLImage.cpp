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

void GLImage::draw(const GLRect& destRect, const GLRect& srcRect)
{
	// set this texture as current
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_);
	
    if (alpha_) {
        // enable alpha blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
	
    // calculate texture coordiantes.
	float srcMinY = (float)srcRect.top / height_;
	float srcMaxY = (float)srcRect.bottom / height_;
	float srcMinX = (float)srcRect.left / width_;
	float srcMaxX = (float)srcRect.right / width_;
    
	// draw the texture
	glBegin(GL_QUADS);
	glTexCoord2f(srcMinX, srcMinY);
    glVertex2i(destRect.left, destRect.top);
	glTexCoord2f(srcMinX, srcMaxY);
    glVertex2i(destRect.left, destRect.bottom);
	glTexCoord2f(srcMaxX, srcMaxY);
    glVertex2i(destRect.right, destRect.bottom);
	glTexCoord2f(srcMaxX, srcMinY);
    glVertex2i(destRect.right, destRect.top);
	glEnd();
	
    if (alpha_) {
        glDisable(GL_BLEND);
    }
	glDisable(GL_TEXTURE_2D);
}

void GLImage::draw(const GLRect& destRect)
{
    draw(destRect, GLRect(0, 0, width_, height_));
}

void GLImage::draw(int x, int y)
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
