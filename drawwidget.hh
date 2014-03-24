#define SIZE 1.5
#define STEP .02
#define BITSTEP .03

#include <QtWidgets/QWidget>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include "contour.hh"

class MySlider: public QSlider {
Q_OBJECT

public:
	MySlider(Qt::Orientation orientation):
		QSlider(orientation)
	{}

public slots:
	/* a hack used by Redraw action */
	void refresh() {
		emit valueChanged(value());
	}
};

class FigureBox: public QSpinBox {
Q_OBJECT

public:
	FigureBox() {
		QSpinBox();
		setRange(2, 10);
		setMinimumWidth(75);
		setSpecialValueText("circle");
	}
};

class DrawWidget: public QWidget {
Q_OBJECT

private:
	R2Rectangle rectangle;
	QPolygonF polygon;
	FigureBox *firstSpinBox;
	FigureBox *secondSpinBox;
	bool drawMode;

	void updatePolygon();

public:
	DrawWidget(FigureBox *firstSpinBox, FigureBox *secondSpinBox):
		rectangle(-SIZE, -SIZE, 2*SIZE, 2*SIZE),
		firstSpinBox(firstSpinBox),
		secondSpinBox(secondSpinBox),
		drawMode(false),
		inputMode(false)
	{
		contour.vertices.clear();
		polygon.clear();
	}

	Contour contour;
	Contour contour1;
	Contour contour2;
	bool inputMode;
	void paintEvent(QPaintEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

public slots:
	void updateContour(int value);
	void nextContour();
};
