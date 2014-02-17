#ifndef GLPOINT_H
#define GLPOINT_H

namespace GL {

class Point {
public:
    Point() : v(0), h(0)
    {
    }
    
    Point(int h_, int v_)
        : v(v_), h(h_)
    {
    }
    
    int v;
    int h;
};
    
}

#endif
