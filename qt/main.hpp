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
    
private:
    GL::Game game;
    QTimer timer;
    
    bool handleKeyEvent(int key, bool down);
};
