#ifndef GL_RESET_DIALOG_H
#define GL_RESET_DIALOG_H

#include "GLFont.h"
#include "GLImage.h"
#include "GLPoint.h"

namespace GL {

class Renderer;

class ResetDialog {
public:
    ResetDialog(const Font& font, const Image& fontImage);
    
    bool isVisible() const;
    
    void show(Renderer *renderer);
    void close();
    
    void draw(Renderer *renderer) const;
    
private:
    const Font& font_;
    const Image& fontImage_;
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
};

}

#endif
