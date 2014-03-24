#include <QtCore/QPointF>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include "drawwidget.hh"

#define SCALE 200
#define OFFSET 50

QPointF qPoint(R2Point *point) {
	return QPointF(SCALE * (SIZE + point->x),
		SCALE * (SIZE + point->y) - OFFSET);
}

R2Point r2Point(QPointF *point) {
	return R2Point(point->x() / SCALE - SIZE,
		(point->y() + OFFSET) / SCALE - SIZE);
}

void DrawWidget::updatePolygon() {
	if (!inputMode) {
		polygon.clear();
		VerticesVector::iterator it;
		for (it = contour.vertices.begin(); it < contour.vertices.end(); ++it)
			polygon << qPoint(&(*it));
	}
}

void DrawWidget::paintEvent(QPaintEvent *event) {
	QWidget::paintEvent(event);
	QPainter painter;
	drawMode = true;
	painter.begin(this);
	painter.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::SquareCap));
#ifdef DRAW_EVERYTHING
	updateContour(0);
	painter.drawPolygon(polygon);
	updateContour(99);
	painter.drawPolygon(polygon);
	painter.setPen(QPen(Qt::gray, 1, Qt::SolidLine, Qt::SquareCap));
	for (short i = 1; i < 10; ++i) {
		updateContour(i * 10);
		painter.drawPolygon(polygon);
	}
#else
	painter.drawPolygon(polygon);
#endif
	painter.end();
	drawMode = false;
}

void DrawWidget::mouseReleaseEvent(QMouseEvent *event) {
	if (!inputMode) return;

	polygon.append(QPointF(event->x(), event->y()));
	repaint();
}

void DrawWidget::updateContour(int value) {
	if (inputMode) return;
#ifdef CONTOURS_INPUT
	uint bitmap_width = rectangle.width() / BITSTEP;
	uint bitmap_height = rectangle.height() / BITSTEP;
	bool *bitmap1 = new bool[bitmap_width * bitmap_height];
	bool *bitmap2 = new bool[bitmap_width * bitmap_height];
	bitmap_fill_from_contour(bitmap1, &contour1, BITSTEP);
	bitmap_fill_from_contour(bitmap2, &contour2, BITSTEP);
	//qDebug() << "Size of bitmap is" << bitmap_width << "*" << bitmap_height;

	Arguments args;
	args.bitmap1 = bitmap1;
	args.bitmap2 = bitmap2;
	args.rectangle = &rectangle;
	args.step = BITSTEP;
	args.position = value;
	contour_new_from_function(
		&contour,
		function_from_bitmap_advanced,
		(void *)(&args),
		STEP,
		&rectangle
	);
#else
	PolygonArguments args;
	args.size1 = firstSpinBox->value();
	args.size2 = secondSpinBox->value();
	args.radius1 = 1;
	args.radius2 = 1;
	args.position = value;
	contour_new_from_function(
		&contour,
		f_polygons,
		(void *)(&args),
		STEP,
		&rectangle
	);
#endif
	updatePolygon();
	if (!drawMode)
		repaint();
}

void DrawWidget::nextContour() {
	VerticesVector *vertices;
	QPolygonF::iterator it;
	if (contour1.vertices.isEmpty()) {
		contour1.rectangle = rectangle;
		vertices = &(contour1.vertices);
	} else {
		contour2.rectangle = rectangle;
		vertices = &(contour2.vertices);
		inputMode = false;
	}
	for (it = polygon.begin(); it != polygon.end(); ++it)
		vertices->append(r2Point(it));
	polygon.clear();
	repaint();
}
