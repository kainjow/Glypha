#include "GLFont.h"
#include "GLImage.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>

GL::Font::Font(const unsigned char* buf, size_t bufLen)
    : first_char_(0)
    , base_(0)
    , lineHeight_(0)
{
    parse(buf, bufLen);
}

void GL::Font::parse(const unsigned char* buf, size_t bufLen)
{
    const std::string bufstr((const char*)buf, bufLen);
    std::stringstream stream(bufstr);
    std::string line;
    int i = 0;
    while (std::getline(stream, line)) {
        std::stringstream ss(line);
        std::string identifier;
        ss >> std::setiosflags(std::ios::skipws) >> identifier;
        if (identifier == "char") {
            std::string key_value;
            while (!ss.eof()) {
                ss >> key_value;
                const std::string::size_type equals = key_value.find('=');
                if (equals != std::string::npos) {
                    std::string key = key_value.substr(0, equals);
                    std::string value = key_value.substr(equals + 1);
                    chars_.resize(static_cast<size_t>(i + 1));
                    Char& curr = chars_.back();
                    if (key == "id") {
                        if (first_char_ == 0) {
                            first_char_ = std::atoi(value.c_str());
                        } else {
                            if (std::atoi(value.c_str()) != (first_char_ + i)) {
                                std::cout << "Unexpected id: " << value << std::endl;
                            }
                        }
                    } else if (key == "x") {
                        curr.x = std::atoi(value.c_str());
                    } else if (key == "y") {
                        curr.y = std::atoi(value.c_str());
                    } else if (key == "width") {
                        curr.width = std::atoi(value.c_str());
                    } else if (key == "height") {
                        curr.height = std::atoi(value.c_str());
                    } else if (key == "xoffset") {
                        curr.xoffset = std::atoi(value.c_str());
                    } else if (key == "yoffset") {
                        curr.yoffset = std::atoi(value.c_str());
                    } else if (key == "xadvance") {
                        curr.xadvance = std::atoi(value.c_str());
                    } else if (key == "page" || key == "chnl") {
                        // ignore
                    } else {
                        std::cout << "Unknown key: " << key << std::endl;
                    }
                }
            }
            ++i;
        } else if (identifier == "info") {
            continue;
        } else if (identifier == "common") {
            std::string key_value;
            while (!ss.eof()) {
                ss >> key_value;
                const std::string::size_type equals = key_value.find('=');
                if (equals != std::string::npos) {
                    std::string key = key_value.substr(0, equals);
                    std::string value = key_value.substr(equals + 1);
                    if (key == "base") {
                        base_ = std::atoi(value.c_str());
                    } else if (key == "lineHeight") {
                        lineHeight_ = std::atoi(value.c_str());
                    }
                }
            }
            continue;
        } else if (identifier == "page") {
            continue;
        } else if (identifier == "chars") {
            continue;
        } else {
            std::cout << "Unknown font identifier: " << identifier << std::endl;
            continue;
        }
    }
}

int GL::Font::safe_char_id(int char_id) const
{
    const int index = static_cast<int>(char_id - first_char_);
    if (index < 0 || index >= static_cast<int>(chars_.size())) {
        return '?';
    }
    return char_id;
}

void GL::Font::drawText(const char *text, int x, int y, const Image& img) const
{
    const size_t len = strlen(text);
    int pos_x = x;
    int pos_y = y;
    for (size_t i = 0; i < len; ++i) {
        const int char_id = safe_char_id(text[i]);
        const int index = static_cast<int>(char_id - first_char_);
        const Char& ch = chars_.at(static_cast<size_t>(index));
        GL::Rect dest(pos_x + ch.xoffset, pos_y + /*base_ +*/ ch.yoffset, ch.width, ch.height);
        img.draw(dest, Rect(ch.x, ch.y, ch.width, ch.height));
        pos_x += ch.xadvance;
    }
}

int GL::Font::measureText(const char *text) const
{
    int width = 0;
    const size_t len = strlen(text);
    for (size_t i = 0; i < len; ++i) {
        const int char_id = safe_char_id(text[i]);
        const int index = static_cast<int>(char_id - first_char_);
        const Char& ch = chars_.at(static_cast<size_t>(index));
        width += ch.xadvance;
    }
    return width;
}

int GL::Font::lineHeight() const
{
    return lineHeight_;
}
