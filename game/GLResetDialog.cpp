#include "GLResetDialog.h"
#include "GLRenderer.h"

GL::ResetDialog::ResetDialog(const Font& font, const Image& fontImage, Callback callback)
    : font_(font)
    , fontImage_(fontImage)
    , callback_(callback)
    , resetDialogVisible(false)
    , resetTitle("Are you sure you want to reset " GL_GAME_NAME "'s scores?")
    , titleWidth(font_.measureText(resetTitle))
    , yes("Yes")
    , yesWidth(font_.measureText(yes))
    , no("No")
    , noWidth(font_.measureText(no))
    , margin(8)
    , buttonYPadding(margin)
    , buttonXPadding(margin * 2)
    , mouseDownInYes(false)
    , mouseDownInNo(false)
    , mouseInYes(false)
    , mouseInNo(false)
{
}

bool GL::ResetDialog::isVisible() const
{
    return resetDialogVisible;
}

void GL::ResetDialog::show(Renderer *renderer)
{
    const Rect bounds = renderer->bounds();
    
    const int dialogHeight = (font_.lineHeight() * 2) + (margin * 3) + buttonYPadding;
    
    dialogRect = Rect(0, 40, titleWidth + (margin * 2), dialogHeight);
    dialogRect.offsetBy((bounds.width() - dialogRect.width()) / 2, 0);
    
    titlePoint = Point(dialogRect.top + margin, dialogRect.left + margin);
    
    const int buttonY = titlePoint.h + font_.lineHeight() + margin;
    resetYesRect = Rect(dialogRect.left + margin, buttonY, yesWidth + buttonXPadding, font_.lineHeight() + buttonYPadding);
    resetNoRect = Rect(resetYesRect.left + resetYesRect.width() + margin, buttonY, noWidth + buttonXPadding, font_.lineHeight() + buttonYPadding);
    
    const int centerXOffset = ((bounds.width() - (resetYesRect.width() + margin + resetNoRect.width())) / 2) - resetYesRect.left;
    resetYesRect.offsetBy(centerXOffset, 0);
    resetNoRect.offsetBy(centerXOffset, 0);
    
    resetDialogVisible = true;
}

void GL::ResetDialog::close()
{
    resetDialogVisible = false;
}

void GL::ResetDialog::draw(Renderer *renderer) const
{
    if (!resetDialogVisible) {
        return;
    }
    
    Renderer *r = renderer;
    
    r->setFillColor(0.92f, 0.92f, 0.92f);
    r->fillRect(dialogRect);
    
    r->setFillColor(0, 0, 0);
    font_.drawText(resetTitle, titlePoint.v, titlePoint.h, fontImage_);
    
    if (mouseInYes) {
        r->setFillColor(28/255.0f, 87/255.0f, 232/255.0f);
    } else {
        r->setFillColor(46/255.0f, 64/255.0f, 1.0);
    }
    r->fillRect(resetYesRect);
    r->setFillColor(1, 1, 1);
    font_.drawText(yes, resetYesRect.left + (buttonXPadding / 2), resetYesRect.top + (buttonYPadding / 2), fontImage_);
    
    if (mouseInNo) {
        r->setFillColor(0.9f, 0.9f, 0.9f);
    } else {
        r->setFillColor(1.0, 1.0, 1.0);
    }
    r->fillRect(resetNoRect);
    r->setFillColor(0.0, 0.0, 0.0);
    font_.drawText(no, resetNoRect.left + (buttonXPadding / 2), resetNoRect.top + (buttonYPadding / 2), fontImage_);
    r->setFillColor(200/255.0f, 200/255.0f, 200/255.0f);
    r->beginLines(1.0, false);
    r->moveTo(resetNoRect.left, resetNoRect.top);
    r->lineTo(resetNoRect.right, resetNoRect.top);
    r->moveTo(resetNoRect.right, resetNoRect.top);
    r->lineTo(resetNoRect.right, resetNoRect.bottom);
    r->moveTo(resetNoRect.right, resetNoRect.bottom);
    r->lineTo(resetNoRect.left, resetNoRect.bottom);
    r->moveTo(resetNoRect.left, resetNoRect.bottom);
    r->lineTo(resetNoRect.left, resetNoRect.top - 1);
    r->endLines();
}

void GL::ResetDialog::handleMouseDownEvent(const Point& point)
{
    mouseDownInYes = mouseInYes = resetYesRect.containsPoint(point);
    mouseDownInNo = mouseInNo = resetNoRect.containsPoint(point);
}

void GL::ResetDialog::handleMouseMovedEvent(const Point& point)
{
    if (mouseDownInYes) {
        mouseInYes = resetYesRect.containsPoint(point);
    }
    if (mouseDownInNo) {
        mouseInNo = resetNoRect.containsPoint(point);
    }
}

void GL::ResetDialog::handleMouseUpEvent(const Point& point)
{
    bool clickedYes = false;
    bool clickedNo = false;
    if (mouseDownInYes && resetYesRect.containsPoint(point)) {
        clickedYes = true;
    } else if (mouseDownInNo && resetNoRect.containsPoint(point)) {
        clickedNo = true;
    }
    mouseDownInYes = mouseInYes = false;
    mouseDownInNo = mouseInNo = false;
    if (clickedNo) {
        close();
    } else if (clickedYes) {
        close();
        callback_();
    }
}
