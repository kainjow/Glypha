#include "GLRect.h"

GLRect::GLRect()
    : top(0), left(0), bottom(0), right(0)
{
}

GLRect::GLRect(int width, int height)
    : top(0), left(0), bottom(width), right(height)
{
}

GLRect::GLRect(int theLeft, int theTop, int width, int height)
    : top(theTop), left(theLeft), bottom(theTop + height), right(theLeft + width)
{
}

void GLRect::set(int theLeft, int theTop, int theRight, int theBottom)
{
    left = theLeft;
    top = theTop;
    right = theRight;
    bottom = theBottom;
}

int GLRect::width() const
{
    return right - left;
}

void GLRect::setWidth(int width)
{
    right = left + width;
}

int GLRect::height() const
{
    return bottom - top;
}

void GLRect::offsetBy(int horizontal, int vertical)
{
    left += horizontal;
    right += horizontal;
    top += vertical;
    bottom += vertical;
}

void GLRect::setSize(int width, int height)
{
    right = left + width;
    bottom = top + height;
}

void GLRect::zeroCorner()		// Offset rect to (0, 0)
{
	right -= left;
	bottom -= top;
	left = 0;
	top = 0;
}

bool GLRect::sect(const GLRect *r2)
{
    const GLRect *r1 = this;
    return (r1->left < r2->right && r1->right > r2->left && r1->top < r2->bottom && r1->bottom > r2->top);
}