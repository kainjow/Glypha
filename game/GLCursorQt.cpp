#include "GLCursor.h"
#include <QApplication>

void GL::Cursor::obscure()
{
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
}

void GL::Cursor::show()
{
    QApplication::restoreOverrideCursor();
}
