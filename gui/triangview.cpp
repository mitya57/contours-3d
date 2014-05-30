#include <QtCore/qmath.h>
#include <QtGui/QPalette>
#include "triangview.h"

static const GLfloat LightPosition[4] = {0.0f, 0.0f, 10.0f, 1.0f};
static const GLfloat LightAmbient[4] = {0.5f, 0.5f, 0.5f, 1.0f};
static const GLfloat LightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};

static const GLfloat RedColor[3]  = {1.0f, 0.4f, 0.4f};

/* TODO: use GL_SMOOTH */

TriangView::TriangView(QWidget *parent):
    QGLWidget(parent), scale(.8)
{
#ifdef DRAW_CUBE
    /* Drawing a cube:
     *
     *    6-------7                  diagonals:
     *   /|      /|
     *  4-------5 |     z    y       0-3 0-5 0-6 3-5 3-6 5-6
     *  | |     | |     |   /
     *  | 2-----|-3     |  /
     *  |/      |/      | /
     *  0-------1       |/______ x
     *
     */

    for (unsigned i = 0; i < 8; ++i) {
        points.push_back(Point3Df(
            (i & 1) ? .5f : -.5f,
            (i & 2) ? .5f : -.5f,
            (i & 4) ? .5f : -.5f
        ));
    }

    triangles.push_back(Triangle3Df(0, 1, 3, 0.f, 0.f, -1.f));
    triangles.push_back(Triangle3Df(0, 2, 3, 0.f, 0.f, -1.f));
    triangles.push_back(Triangle3Df(0, 1, 5, 0.f, -1.f, 0.f));
    triangles.push_back(Triangle3Df(0, 4, 5, 0.f, -1.f, 0.f));
    triangles.push_back(Triangle3Df(0, 2, 6, -1.f, 0.f, 0.f));
    triangles.push_back(Triangle3Df(0, 4, 6, -1.f, 0.f, 0.f));
    triangles.push_back(Triangle3Df(3, 1, 5, 1.f, 0.f, 0.f));
    triangles.push_back(Triangle3Df(3, 7, 5, 1.f, 0.f, 0.f));
    triangles.push_back(Triangle3Df(3, 2, 6, 0.f, 1.f, 0.f));
    triangles.push_back(Triangle3Df(3, 7, 6, 0.f, 1.f, 0.f));
    triangles.push_back(Triangle3Df(5, 4, 6, 0.f, 0.f, 1.f));
    triangles.push_back(Triangle3Df(5, 7, 6, 0.f, 0.f, 1.f));
#else
    fillVectors(points, triangles);
#endif
}

void TriangView::drawTriangles() {
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, RedColor);
    glBegin(GL_TRIANGLES);
    for (unsigned i = 0; i < triangles.size(); ++i) {
        Vector3Df normal = triangles[i].normal;
        glNormal3f(normal.x, normal.y, normal.z);
        for (unsigned j = 0; j < 3; ++j) {
            Point3Df point = points[triangles[i].ind[j]];
            glVertex3f(point.x, point.y, point.z);
        }
    }
    glEnd();
}

void TriangView::initializeGL() {
    qglClearColor(QPalette().color(QPalette::Window));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

    drawTriangles();
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
