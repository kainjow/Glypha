#ifndef GLRECT
#define GLRECT

namespace GL {

class Point;

class Rect {
public:
    Rect();
    Rect(int width, int height);
    Rect(int theLeft, int theTop, int width, int height);
    
    void set(int theLeft, int theTop, int theRight, int theBottom);
    
    int width() const;
    void setWidth(int width);
    int height() const;
    
    void offsetBy(int horizontal, int vertical);
    
    void setSize(int width, int height);
    
    void zeroCorner();
    
    bool sect(const Rect& r2) const;
    
    void inset(int dh, int dv);
    
    bool containsPoint(const Point& point) const;

    int top, left, bottom, right;
};

}

#endif
