#include "GLImage.h"
#include <QImage>
#include <QtOpenGL>

void GL::Image::load(const unsigned char *buf, size_t bufSize)
{
    QImage img;
    if (img.loadFromData(buf, bufSize)) {
        img = img.mirrored(false, true);
        QImage glImg(QGLWidget::convertToGLFormat(img));
        if (!glImg.isNull()) {
            width_ = glImg.width();
            height_ = glImg.height();
            loadTextureData_(glImg.bits(), GL_RGBA, img.hasAlphaChannel());
        }
    }
}
