// sudo apt-get install libqt4-dev libqt4-opengl-dev

#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QtOpenGL>

class GLWidget : public QGLWidget
{
public:
    GLWidget(QWidget *parent = 0)
        : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
    {
    }

protected:
    void initializeGL() {
        glClearColor(0.0, 0.0, 0.0, 0.0);
    }
    void paintGL() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    void resizeGL(int width, int height) {
        (void)width; (void)height;
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow win;
    win.setFixedSize(640, 460);
    win.setWindowTitle("Glypha III");
    
    QWidget *mainWidget = new QWidget;
    GLWidget *glwid = new GLWidget;

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(glwid);
    mainWidget->setLayout(layout);
    win.setCentralWidget(mainWidget);

    QMenu *fileMenu = new QMenu("&File");
    fileMenu->addAction("&Quit");
    win.menuBar()->addMenu(fileMenu);

    win.show();
    return app.exec();
}
