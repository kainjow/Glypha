#ifndef GL_RESET_DIALOG_H
#define GL_RESET_DIALOG_H

#include "GLFont.h"
#include "GLImage.h"
#include "GLPoint.h"
#include <functional>

namespace GL {

class Renderer;

class ResetDialog {
public:
    using Callback = std::function<void()>;
    ResetDialog(const Font& font, const Image& fontImage, Callback callback);
    
    bool isVisible() const;
    
    void show(Renderer *renderer);
    void close();
    
    void draw(Renderer *renderer) const;

    void handleMouseDownEvent(const Point& point);
    void handleMouseMovedEvent(const Point& point);
    void handleMouseUpEvent(const Point& point);

private:
    const Font& font_;
    const Image& fontImage_;
    Callback callback_;
    bool resetDialogVisible;
    const char *resetTitle;
    int titleWidth;
    Point titlePoint;
    const char *yes;
    int yesWidth;
    const char *no;
    int noWidth;
    int margin;
    int buttonYPadding;
    int buttonXPadding;
    Rect dialogRect;
    Rect resetYesRect;
    Rect resetNoRect;
    bool mouseDownInYes;
    bool mouseDownInNo;
    bool mouseInYes;
    bool mouseInNo;
};

}

#endif
