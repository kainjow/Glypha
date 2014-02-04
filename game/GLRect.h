#ifndef GLRECT
#define GLRECT

class GLRect {
public:
    GLRect();
    GLRect(int width, int height);
    GLRect(int theLeft, int theTop, int width, int height);
    
    void set(int theLeft, int theTop, int theRight, int theBottom);
    
    int width() const;
    void setWidth(int width);
    int height() const;
    
    void offsetBy(int horizontal, int vertical);
    
    void setSize(int width, int height);
    
    void zeroCorner();
    
    bool sect(const GLRect *r2);

    int top, left, bottom, right;
};

#endif
