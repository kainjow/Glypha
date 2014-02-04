TARGET = Glypha
QT += opengl
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .
DEFINES += GLYPHA_QT

SOURCES += main.cpp \
    ../game/GLImage.cpp \
    ../game/GLRect.cpp \
    ../game/GLRenderer.cpp \
    ../game/GLResources.cpp \
    ../game/GLSounds.cpp \
    ../game/GLUtils.cpp
