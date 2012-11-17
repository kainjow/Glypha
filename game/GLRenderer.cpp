//
//  GLRenderer.cpp
//  Glypha
//
//  Created by Kevin Wojniak on 7/19/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#include "GLRenderer.h"
#if __APPLE__
#include <OpenGL/gl.h>
#else
#include <windows.h>
#include <gl/gl.h>
#endif

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
	glVertex2i(rect.left(), rect.bottom());
	glVertex2i(rect.left(), rect.top());
	glVertex2i(rect.right(), rect.top());
	glVertex2i(rect.right(), rect.bottom());
	glEnd();
}

void GLRenderer::setFillColor(int red, int green, int blue)
{
    glColor3f((GLfloat)red, (GLfloat)green, (GLfloat)blue);
}

GLRect GLRenderer::bounds()
{
    return bounds_;
}

void GLRenderer::beginLines(float lineWidth)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
}

void GLRenderer::endLines()
{
	glEnd();
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_BLEND);
}

void GLRenderer::drawLine(int h1, int v1, int h2, int v2)
{
	glVertex2s(h1, v1);
	glVertex2s(h2, v2);
}

void GLRenderer::moveTo(int h, int v)
{
    lineStart_.h = h;
    lineStart_.v = v;
}

void GLRenderer::lineTo(int h, int v)
{
	glVertex2s(lineStart_.h, lineStart_.v);
	glVertex2s(h, v);
}
