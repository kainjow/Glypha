//
//  GLRenderer.cpp
//  Glypha
//
//  Created by Kevin Wojniak on 7/19/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#include "GLRenderer.h"
#if __APPLE__
#include <OpenGL/OpenGL.h>
#endif
#include <cstdio>

GLRenderer::GLRenderer()
{
}

GLRenderer::~GLRenderer()
{
}

void GLRenderer::resize(int width, int height)
{
    GLsizei w = width, h = height;
	
	glViewport(0, 0, w, h);
    
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, w, h, 0.0, -1.0, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    bounds_.setSize(width, height);
}

void GLRenderer::clear()
{
    if (didPrepare_ == false) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepth(1.0f);
        didPrepare_ = true;
    }
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
}

void GLRenderer::fillRect(const GLRect &rect)
{
	glBegin(GL_QUADS);
	glVertex2f(rect.left(), rect.bottom());
	glVertex2f(rect.left(), rect.top());
	glVertex2f(rect.right(), rect.top());
	glVertex2f(rect.right(), rect.bottom());
	glEnd();
}

void GLRenderer::setFillColor(int red, int green, int blue)
{
    glColor3f(red, green, blue);
}

GLRect GLRenderer::bounds()
{
    return bounds_;
}
