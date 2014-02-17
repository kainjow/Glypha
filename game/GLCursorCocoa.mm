#include "GLCursor.h"
#include <AppKit/AppKit.h>

void GL::Cursor::obscure()
{
    [NSCursor setHiddenUntilMouseMoves:YES];
}

void GL::Cursor::show()
{
    [NSCursor setHiddenUntilMouseMoves:NO];
}
