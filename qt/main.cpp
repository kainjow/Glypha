// sudo apt-get install libqt4-dev libqt4-opengl-dev

#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QInputDialog>
#include "main.hpp"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
    , game_(callback, highScoreNameCallback, this)
{
    timer_.setInterval(1000.0 / 30.0);
    connect(&timer_, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer_.start();
    
    setFocusPolicy(Qt::StrongFocus);
}

void GLWidget::callback(GL::Game::Event event, void *context)
{
    GLWidget *widget = (GLWidget*)context;
    MainWindow *win = (MainWindow*)widget->window();
    win->callback(event);
}

void GLWidget::highScoreNameCallback(const char *name, int place, void *context)
{
    GLWidget *widget = (GLWidget*)context;
    widget->highScoreNameCallback(name, place);
}

void GLWidget::highScoreNameCallback(const char *name, int place)
{
    QString label = tr("Your score #%1 of the ten best! Enter your name (15 chars.).").arg(place);
    QString input;
    bool ok;
    do {
        input = QInputDialog::getText(this, tr("High Score"), label, QLineEdit::Normal, QString::fromStdString(name), &ok);
    } while (!ok);
    game_.processHighScoreName(input.toStdString().c_str(), place);
}

void GLWidget::paintGL()
{
    game_.run();
}

void GLWidget::resizeGL(int width, int height)
{
    game_.renderer()->resize(width, height);
}

void GLWidget::newGame()
{
    game_.newGame();
}

bool GLWidget::pauseGame()
{
    game_.pauseResumeGame();
    return game_.paused();
}

void GLWidget::endGame()
{
    game_.endGame();
}

void GLWidget::showHelp()
{
    game_.showHelp();
}

void GLWidget::showAbout()
{
    game_.showAbout();
}

void GLWidget::showHighScores()
{
    game_.showHighScores();
}

void GLWidget::resetHighScores()
{
    QMessageBox msgbox(QMessageBox::Icon::Warning, tr("Reset Scores"),
        tr("Are you sure you want to reset " GL_GAME_NAME "'s scores?"),
        QMessageBox::Yes | QMessageBox::No, this, Qt::Dialog);
    if (msgbox.exec() == QMessageBox::Yes) {
        game_.resetHighScores();
    }
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
            game_.handleKeyDownEvent(gameKey);
        } else {
            game_.handleKeyUpEvent(gameKey);
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
    game_.handleMouseDownEvent(GL::Point(event->pos().x(), event->pos().y()));
}

MainWindow::MainWindow()
    : QMainWindow()
{
    setWindowTitle(GL_GAME_NAME);

    QWidget *mainWidget = new QWidget;
    glwid_ = new GLWidget;
    glwid_->setFixedSize(640, 460);
    
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(glwid_);
    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);
    
    QMenu *fileMenu = new QMenu("&File");
    newAction_ = fileMenu->addAction("&New Game");
    QObject::connect(newAction_, SIGNAL(triggered()), glwid_, SLOT(newGame()));
    newAction_->setShortcut(QKeySequence("Ctrl+N"));
    pauseAction_ = fileMenu->addAction("&Pause Game\tCtrl+P");
    QObject::connect(pauseAction_, SIGNAL(triggered()), this, SLOT(pauseGame()));
    pauseAction_->setShortcut(QKeySequence("Ctrl+P"));
    pauseAction_->setEnabled(false);
    endAction_ = fileMenu->addAction("&End Game\tCtrl+E");
    QObject::connect(endAction_, SIGNAL(triggered()), glwid_, SLOT(endGame()));
    endAction_->setShortcut(QKeySequence("Ctrl+E"));
    endAction_->setEnabled(false);
    fileMenu->addSeparator();
    QAction *quitMenu = fileMenu->addAction("&Quit");
    QObject::connect(quitMenu, SIGNAL(triggered()), qApp, SLOT(quit()));
    quitMenu->setShortcut(QKeySequence("Ctrl+Q"));
    menuBar()->addMenu(fileMenu);
    
    QMenu *optionsMenu = new QMenu("&Options");
    helpAction_ = optionsMenu->addAction("&Help");
    QObject::connect(helpAction_, SIGNAL(triggered()), glwid_, SLOT(showHelp()));
    helpAction_->setShortcut(QKeySequence("Ctrl+H"));
    optionsMenu->addSeparator();
    scoresAction_ = optionsMenu->addAction("High &Scores");
    QObject::connect(scoresAction_, SIGNAL(triggered()), glwid_, SLOT(showHighScores()));
    scoresAction_->setShortcut(QKeySequence("Ctrl+S"));
    resetAction_ = optionsMenu->addAction("&Reset Scores...");
    QObject::connect(resetAction_, SIGNAL(triggered()), glwid_, SLOT(resetHighScores()));
    optionsMenu->addSeparator();
    aboutAction_ = optionsMenu->addAction("&About " GL_GAME_NAME);
    QObject::connect(aboutAction_, SIGNAL(triggered()), glwid_, SLOT(showAbout()));
    menuBar()->addMenu(optionsMenu);
}

void MainWindow::pauseGame()
{
    if (glwid_->pauseGame()) {
        pauseAction_->setText("&Resume Game\tCtrl+R");
        pauseAction_->setShortcut(QKeySequence("Ctrl+R"));
    } else {
        pauseAction_->setText("&Pause Game\tCtrl+P");
        pauseAction_->setShortcut(QKeySequence("Ctrl+P"));
    }
}

void MainWindow::callback(GL::Game::Event event)
{
    switch (event) {
        case GL::Game::EventStarted:
            newAction_->setEnabled(false);
            pauseAction_->setEnabled(true);
            endAction_->setEnabled(true);
            helpAction_->setEnabled(false);
            scoresAction_->setEnabled(false);
            resetAction_->setEnabled(false);
            aboutAction_->setEnabled(false);
            break;
        case GL::Game::EventEnded:
            newAction_->setEnabled(true);
            pauseAction_->setEnabled(false);
            pauseAction_->setText("&Pause Game\tCtrl+P");
            pauseAction_->setShortcut(QKeySequence("Ctrl+P"));
            endAction_->setEnabled(false);
            helpAction_->setEnabled(true);
            scoresAction_->setEnabled(true);
            resetAction_->setEnabled(true);
            aboutAction_->setEnabled(true);
            break;
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("kainjow");
    app.setOrganizationDomain("kainjow.com");
    app.setApplicationName(GL_GAME_NAME);
    MainWindow win;
    win.show();
    return app.exec();
}
