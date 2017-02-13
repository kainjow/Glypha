#ifndef GL_FONT_H
#define GL_FONT_H

#include <cstddef>
#include <vector>

namespace GL {

class Image;

class Font {
public:
    
    Font(const unsigned char* buf, size_t bufLen);
    
    void drawText(const char *text, int x, int y, const Image& img) const;
    
    int measureText(const char *text) const;
    
    int lineHeight() const;
    
private:
    void parse(const unsigned char* buf, size_t bufLen);

    struct Char {
        int x;
        int y;
        int width;
        int height;
        int xoffset;
        int yoffset;
        int xadvance;
    };

    std::vector<Char> chars_;
    int first_char_;
    int base_;
    int lineHeight_;
};

}

#endif
