#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QMainWindow>
#include "triangview.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QMainWindow window;
    TriangView view;
    QToolBar toolBar;
    QSlider slider(Qt::Horizontal);

    toolBar.addWidget(&slider);
    window.resize(view.size());
    window.setWindowIcon(QIcon::fromTheme("application-x-executable"));
    window.setWindowTitle("3D interpolation");
    window.setCentralWidget(&view);
 // window.addToolBar(Qt::BottomToolBarArea, &toolBar);
    window.show();

    return app.exec();
}
