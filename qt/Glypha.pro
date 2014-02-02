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
    ../game/GLSounds.cpp \
    ../game/GLUtils.cpp \
    ../themes/glyphaiii/images/background.c \
    ../themes/glyphaiii/images/numbers.c \
    ../themes/glyphaiii/images/platforms.c \
    ../themes/glyphaiii/images/player.c \
    ../themes/glyphaiii/images/playerIdle.c \
    ../themes/glyphaiii/images/torches.c \
    ../themes/glyphaiii/sounds/bird.c \
    ../themes/glyphaiii/sounds/flap.c \
    ../themes/glyphaiii/sounds/grate.c \
    ../themes/glyphaiii/sounds/screech.c \
    ../themes/glyphaiii/sounds/walk.c
