#include <QtCore/Qt>
#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtGui/QIcon>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QAction>
#include "drawwidget.hh"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QMainWindow window;
	QWidget centralWidget;
	QToolBar toolBar;
	toolBar.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	QAction nextAction(QIcon::fromTheme("go-next"), "Next", &window);
	QAction redrawAction(QIcon::fromTheme("view-refresh-symbolic"), "Redraw", &window);

	window.resize(600, 600);
	window.setWindowIcon(QIcon::fromTheme("application-x-executable"));
	window.setWindowTitle("Contours Interpolation");

	FigureBox firstSpinBox;
	FigureBox secondSpinBox;
	DrawWidget drawwgt(&firstSpinBox, &secondSpinBox);
	QLabel separatorLabel;
	separatorLabel.setText(QString::fromUtf8("\xe2\x86\x92"));
	separatorLabel.setMargin(5);

	firstSpinBox.setValue(5);
	secondSpinBox.setValue(3);

#ifdef CONTOURS_INPUT
	toolBar.addAction(&nextAction);
	toolBar.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	drawwgt.inputMode = true;
#else
	toolBar.addWidget(&firstSpinBox);
	toolBar.addWidget(&separatorLabel);
	toolBar.addWidget(&secondSpinBox);
	toolBar.addSeparator();
	toolBar.addAction(&redrawAction);
	drawwgt.updateContour(0);
#endif

	window.addToolBar(Qt::BottomToolBarArea, &toolBar);

	QVBoxLayout layout;
	MySlider slider(Qt::Horizontal);

	QObject::connect(&slider, SIGNAL(valueChanged(int)),
		&drawwgt, SLOT(updateContour(int)));
	QObject::connect(&nextAction, SIGNAL(triggered()),
		&drawwgt, SLOT(nextContour()));
	QObject::connect(&redrawAction, SIGNAL(triggered()),
		&slider, SLOT(refresh()));

	layout.addWidget(&slider);
	layout.addWidget(&drawwgt);
	centralWidget.setLayout(&layout);
	window.setCentralWidget(&centralWidget);
	window.show();

	return a.exec();
}
