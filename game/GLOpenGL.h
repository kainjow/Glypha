#ifndef GLOPENGL_H
#define GLOPENGL_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#elif defined(_WIN32)
#include <windows.h>
#include <gl/gl.h>
#elif defined(__HAIKU__)
#include <GL/gl.h>
#elif defined(GLYPHA_QT)
#include <QtOpenGL>
#endif

#endif
