#ifndef GLPOINT_H
#define GLPOINT_H

class GLPoint {
public:
    GLPoint() : v(0), h(0)
    {
    }
    
    GLPoint(int h_, int v_)
        : v(v_), h(h_)
    {
    }
    
    int v;
    int h;
};

#endif
