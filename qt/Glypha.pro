TARGET = Glypha
QT += opengl
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
    ../game/GLSounds.cpp \
    ../game/GLUtils.cpp
    
HEADERS += main.hpp
