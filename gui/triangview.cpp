#include <QtCore/qmath.h>
#include <QtGui/QPalette>
#include "triangview.h"

static const GLfloat LightPosition[4] = {0.0f, 0.0f, 10.0f, 1.0f};
static const GLfloat LightAmbient[4] = {0.5f, 0.5f, 0.5f, 1.0f};
static const GLfloat LightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};

void TriangView::initializeGL() {
    qglClearColor(QPalette().color(QPalette::Window));
    glEnable(GL_DEPTH_CLAMP);
}

void TriangView::resizeGL(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat ratio = (GLfloat)height / width;

    if (width > height) {
        glOrtho(-1./ratio, 1./ratio, -1., 1., -10., 1.);
    } else {
        glOrtho(-1., 1., -ratio, ratio, -10., 1.);
    }

    glViewport(0, 0, width, height);
}

void TriangView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(scale, scale, scale);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(zRot, 0.0f, 0.0f, 1.0f);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
    glEnable(GL_LIGHT1);
}

void TriangView::mousePressEvent(QMouseEvent *event) {
    mousePosition = event->pos();
}

void TriangView::mouseMoveEvent(QMouseEvent *event) {
    xRot += 180 / scale * (GLfloat)(event->y() - mousePosition.y()) / height();
    zRot += 180 / scale * (GLfloat)(event->x() - mousePosition.x()) / width();

    mousePosition = event->pos();
    updateGL();
}

void TriangView::wheelEvent(QWheelEvent *event) {
    int delta = event->delta();
    scale *= qExp(double(delta) / 2048);
    repaint();
}
