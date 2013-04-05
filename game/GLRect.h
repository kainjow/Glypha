//
//  GLRect.h
//  Glypha
//
//  Created by Kevin Wojniak on 7/19/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#ifndef GLRECT
#define GLRECT

class GLRect {
public:
    GLRect();
    GLRect(int width, int height);
    GLRect(int left, int top, int width, int height);
    
    void set(int left, int top, int right, int bottom);
    
    int top() const;
    void setTop(int top);
    int left() const;
    void setLeft(int left);
    int bottom() const;
    void setBottom(int bottom);
    int right() const;
    void setRight(int right);
    
    int width() const;
    void setWidth(int width);
    int height() const;
    
    void offsetBy(int horizontal, int vertical);
    
    void setSize(int width, int height);
    
    void zeroCorner();
    
    bool sect(const GLRect *r2);

private:
    int top_, left_, bottom_, right_;
};

#endif
