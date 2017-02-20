#include <QtOpenGL>
#include <QTimer>
#include "GLGame.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);

protected:
    void paintGL();
    void resizeGL(int width, int height);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    
private slots:
    void newGame();
    void endGame();
    void showHelp();
    void showAbout();
    
private:
    GL::Game game_;
    QTimer timer_;
    
    bool handleKeyEvent(int key, bool down);
};
