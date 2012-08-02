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
    
    ~GLRect();
    
    int top() const;
    int left() const;
    int bottom() const;
    int right() const;
    
    int width() const;
    void setWidth(int width);
    int height() const;
    
    void offsetBy(int horizontal, int vertical);
    
    void setSize(int width, int height);

private:
    int top_, left_, bottom_, right_;
};

#endif
