//
//  GLRect.cpp
//  Glypha
//
//  Created by Kevin Wojniak on 7/19/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#include "GLRect.h"

GLRect::GLRect()
    : top_(0), left_(0), bottom_(0), right_(0)
{
}

GLRect::GLRect(int width, int height)
    : top_(0), left_(0), bottom_(width), right_(height)
{
}

GLRect::GLRect(int left, int top, int width, int height)
    : top_(top), left_(left), bottom_(top + height), right_(left + width)
{
}

void GLRect::set(int left, int top, int right, int bottom)
{
    left_ = left;
    top_ = top;
    right_ = right;
    bottom_ = bottom;
}

int GLRect::top() const
{
    return top_;
}

void GLRect::setTop(int top)
{
    top_ = top;
}

int GLRect::left() const
{
    return left_;
}

void GLRect::setLeft(int left)
{
    left_ = left;
}

int GLRect::bottom() const
{
    return bottom_;
}

void GLRect::setBottom(int bottom)
{
    bottom_ = bottom;
}

int GLRect::right() const
{
    return right_;
}

void GLRect::setRight(int right)
{
    right_ = right;
}

int GLRect::width() const
{
    return right_ - left_;
}

void GLRect::setWidth(int width)
{
    right_ = left_ + width;
}

int GLRect::height() const
{
    return bottom_ - top_;
}

void GLRect::offsetBy(int horizontal, int vertical)
{
    left_ += horizontal;
    right_ += horizontal;
    top_ += vertical;
    bottom_ += vertical;
}

void GLRect::setSize(int width, int height)
{
    right_ = left_ + width;
    bottom_ = top_ + height;
}

void GLRect::zeroCorner()		// Offset rect to (0, 0)
{
	right_ -= left_;
	bottom_ -= top_;
	left_ = 0;
	top_ = 0;
}

bool GLRect::sect(const GLRect *r2)
{
    const GLRect *r1 = this;
    return (r1->left() < r2->right() && r1->right() > r2->left() && r1->top() < r2->bottom() && r1->bottom() > r2->top());
}