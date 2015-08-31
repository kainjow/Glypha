TARGET = Glypha
QT += opengl multimedia
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += . ../game
DEFINES += GLYPHA_QT

SOURCES += main.cpp \
    ../game/GLCursorQt.cpp \
    ../game/GLGame.cpp \
    ../game/GLImage.cpp \
    ../game/GLImage_Qt.cpp \
    ../game/GLRect.cpp \
    ../game/GLRenderer.cpp \
    ../game/GLResources.cpp \
    ../game/GLSoundsQt.cpp \
    ../game/GLSoundsQtImp.cpp \
    ../game/GLBufferReader.cpp \
    ../game/GLUtils.cpp

HEADERS += main.hpp \
    ../game/GLCursor.h \
    ../game/GLGame.h \
    ../game/GLImage.h \
    ../game/GLRect.h \
    ../game/GLRenderer.h \
    ../game/GLResources.h \
    ../game/GLSounds.h \
    ../game/GLSoundsQtImp.h \
    ../game/GLBufferReader.h \
    ../game/GLUtils.h

CONFIG += c++11
