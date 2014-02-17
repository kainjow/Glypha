#include "GLRect.h"

GL::Rect::Rect()
    : top(0), left(0), bottom(0), right(0)
{
}

GL::Rect::Rect(int width, int height)
    : top(0), left(0), bottom(width), right(height)
{
}

GL::Rect::Rect(int theLeft, int theTop, int width, int height)
    : top(theTop), left(theLeft), bottom(theTop + height), right(theLeft + width)
{
}

void GL::Rect::set(int theLeft, int theTop, int theRight, int theBottom)
{
    left = theLeft;
    top = theTop;
    right = theRight;
    bottom = theBottom;
}

int GL::Rect::width() const
{
    return right - left;
}

void GL::Rect::setWidth(int width)
{
    right = left + width;
}

int GL::Rect::height() const
{
    return bottom - top;
}

void GL::Rect::offsetBy(int horizontal, int vertical)
{
    left += horizontal;
    right += horizontal;
    top += vertical;
    bottom += vertical;
}

void GL::Rect::setSize(int width, int height)
{
    right = left + width;
    bottom = top + height;
}

void GL::Rect::zeroCorner()		// Offset rect to (0, 0)
{
	right -= left;
	bottom -= top;
	left = 0;
	top = 0;
}

bool GL::Rect::sect(const GL::Rect& r2) const
{
    return (left < r2.right && right > r2.left && top < r2.bottom && bottom > r2.top);
}

void GL::Rect::inset(int dh, int dv)
{
    left += dh;
    right -= (dh * 2);
    top += dv;
    bottom -= (dv * 2);
}

