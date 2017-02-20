// sudo apt-get install libqt4-dev libqt4-opengl-dev

#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QMenuBar>
#include "main.hpp"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
    , game(nullptr, nullptr, nullptr)
{
    timer.setInterval(1000.0 / 30.0);
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start();
    
    setFocusPolicy(Qt::StrongFocus);
}

void GLWidget::paintGL()
{
    game.run();
}

void GLWidget::resizeGL(int width, int height)
{
    game.renderer()->resize(width, height);
}

void GLWidget::newGame()
{
    game.newGame();
}

void GLWidget::endGame()
{
    game.endGame();
}

void GLWidget::showHelp()
{
    game.showHelp();
}

bool GLWidget::handleKeyEvent(int key, bool down)
{
    GL::Game::Key gameKey = GL::Game::KeyNone;
    switch (key) {
        case Qt::Key_Up:
            gameKey = GL::Game::KeyUpArrow;
            break;
        case Qt::Key_Down:
            gameKey = GL::Game::KeyDownArrow;
            break;
        case Qt::Key_Left:
            gameKey = GL::Game::KeyLeftArrow;
            break;
        case Qt::Key_Right:
            gameKey = GL::Game::KeyRightArrow;
            break;
        case Qt::Key_Space:
            gameKey = GL::Game::KeySpacebar;
            break;
        case Qt::Key_PageUp:
            gameKey = GL::Game::KeyPageUp;
            break;
        case Qt::Key_PageDown:
            gameKey = GL::Game::KeyPageDown;
            break;
    }
    if (gameKey) {
        if (down) {
            game.handleKeyDownEvent(gameKey);
        } else {
            game.handleKeyUpEvent(gameKey);
        }
        return true;
    }
    return false;
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (!handleKeyEvent(event->key(), true)) {
        QGLWidget::keyPressEvent(event);
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (!handleKeyEvent(event->key(), false)) {
        QGLWidget::keyReleaseEvent(event);
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    game.handleMouseDownEvent(GL::Point(event->pos().x(), event->pos().y()));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow win;
    win.setWindowTitle(GL_GAME_NAME);
    
    QWidget *mainWidget = new QWidget;
    GLWidget *glwid = new GLWidget;
    glwid->setFixedSize(640, 460);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(glwid);
    mainWidget->setLayout(layout);
    win.setCentralWidget(mainWidget);

    QMenu *fileMenu = new QMenu("&File");
    QAction *newGameMenu = fileMenu->addAction("&New Game");
    QObject::connect(newGameMenu, SIGNAL(triggered()), glwid, SLOT(newGame()));
    newGameMenu->setShortcut(QKeySequence("Ctrl+N"));
    QAction *endGameMenu = fileMenu->addAction("&End Game\tCtrl+E");
    QObject::connect(endGameMenu, SIGNAL(triggered()), glwid, SLOT(endGame()));
    endGameMenu->setShortcut(QKeySequence("Ctrl+E"));
    fileMenu->addSeparator();
    QAction *quitMenu = fileMenu->addAction("&Quit");
    QObject::connect(quitMenu, SIGNAL(triggered()), qApp, SLOT(quit()));
    quitMenu->setShortcut(QKeySequence("Ctrl+Q"));
    win.menuBar()->addMenu(fileMenu);
    
    QMenu *optionsMenu = new QMenu("&Options");
    QAction *helpAction = optionsMenu->addAction("&Help");
    QObject::connect(helpAction, SIGNAL(triggered()), glwid, SLOT(showHelp()));
    helpAction->setShortcut(QKeySequence("Ctrl+H"));
    win.menuBar()->addMenu(optionsMenu);

    win.show();
    return app.exec();
}
