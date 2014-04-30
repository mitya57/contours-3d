#ifndef __TRIANGVIEW_H
#define __TRIANGVIEW_H

#include <QtCore/QPoint>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtOpenGL/QGLWidget>

class TriangView: public QGLWidget {
Q_OBJECT

private:
    QPoint mousePosition;
    double xRot;
    double yRot;
    double zRot;
    double scale;

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
};

#endif /* __TRIANGVIEW_H */
