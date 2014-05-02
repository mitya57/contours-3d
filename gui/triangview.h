#ifndef __TRIANGVIEW_H
#define __TRIANGVIEW_H

#include <QtCore/QPoint>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtOpenGL/QGLWidget>
#include "triang.h"

class TriangView: public QGLWidget {
Q_OBJECT

public:
    TriangView(QWidget *parent = 0);

private:
    QPoint mousePosition;
    double xRot;
    double yRot;
    double zRot;
    double scale;

    std::vector<Point3Df> points;
    std::vector<Triangle3Df> triangles;

    void drawTriangles();
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
};

#endif /* __TRIANGVIEW_H */
