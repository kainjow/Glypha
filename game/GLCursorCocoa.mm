#include "GLCursor.h"
#include <AppKit/AppKit.h>

void GLCursor::obscure()
{
    [NSCursor setHiddenUntilMouseMoves:YES];
}

void GLCursor::show()
{
    [NSCursor setHiddenUntilMouseMoves:NO];
}
