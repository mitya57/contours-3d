#include <QtWidgets/QApplication>
#include "triangview.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    TriangView view;
    return app.exec();
}
