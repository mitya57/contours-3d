#include <cmath>
#include <cassert>
#include "contour.hh"
#include "R2Graph.h"

#ifndef NO_QT
#include <QtCore/QDebug>
#endif

/* TODO: use valuemaps instead of bitmaps */

double f_polygon(R2Point p, uint n, double radius) {
	p.x = fabs(p.x);
	double arg = atan2(p.x, p.y);
	double mod = sqrt(p.x * p.x + p.y * p.y);
	double angle = 2 * M_PI / n;
	uint sector = arg / angle;
	double newarg = arg - angle * sector;
	p.x = mod * cos(newarg);
	p.y = mod * sin(newarg);
	double nom = sin(angle) * (radius - p.x) + p.y * (cos(angle) - 1);
	return nom;
}

double f_circle(R2Point point, double radius = 1) {
	return radius - sqrt(point.x * point.x + point.y * point.y);
}

#if 0
double f_square(R2Point point) {
	return 1 - MAX(fabs(point.x), fabs(point.y));
}

double f_square_to_circle(R2Point point, void *arg) {
	double pos = double(*((int *)arg)) / 99;
	return f_square(point) * pos + f_circle(point) * (1 - pos);
}
#endif

double f_polygons(R2Point point, void *arg) {
	PolygonArguments *args = (PolygonArguments *)arg;
	double val1, val2;
	if (args->size1 > 2)
		val1 = f_polygon(point, args->size1, args->radius1);
	else
		val1 = f_circle(point, args->radius1);
	if (args->size2 > 2)
		val2 = f_polygon(point, args->size2, args->radius2);
	else
		val2 = f_circle(point, args->radius2);
	double pos = double(args->position) / 99;
	return val2 * pos + val1 * (1 - pos);
}

R2Vector polar_vector(double angle, double size, bool reversed=false) {
	R2Vector result(size*cos(angle*M_PI), size*sin(angle*M_PI));
	if (reversed)
		result = result * (-1);
	return result;
}

bool compare_signs(double value1, double value2) {
	if (value1 > 0 && value2 > 0)
		return true;
	if (value1 < 0 && value2 < 0)
		return true;
	return false;
}

void contour_new_from_function(Contour *result, double function(R2Point, void *),
void *arg, double step, R2Rectangle *rectangle) {
	// TODO: use better algorhithm to find the starting point
	R2Point point(
		(rectangle->getXMin() + rectangle->getXMax()) / 2,
		(rectangle->getYMin() + rectangle->getYMax()) / 2
	), point1, point2;
	double value1, value2;
	R2Vector initial_step(0, -step/3); // going to bottom
	while (function(point, arg) >= 0)
		point += initial_step;
	result->vertices.clear();

	bool reversed = true;
	R2Point triangle_top = point;
	Side side = SIDE_120_DEG, new_side;
	while (true) {
		// push next point
		if (side == SIDE_0_DEG) {
			point1 = triangle_top - polar_vector(2./3, step, reversed);
			point2 = triangle_top - polar_vector(1./3, step, reversed);
		} else if (side == SIDE_60_DEG) {
			point1 = triangle_top;
			point2 = triangle_top - polar_vector(1./3, step, reversed);
		} else {
			point1 = triangle_top;
			point2 = triangle_top - polar_vector(2./3, step, reversed);
		}
		//qDebug() << "side is" << side;
		//qDebug() << "point1 value is" << function(point1, arg);
		//qDebug() << "point2 value is" << function(point2, arg);
		assert(function(point1, arg) * function(point2, arg) < 0);
		value1 = fabs(function(point1, arg));
		value2 = fabs(function(point2, arg));
		point = point1 + (point2 - point1) * (value1 / (value1 + value2));
		if (result->vertices.size() && point == result->vertices.at(0))
			break;
		//qDebug() << "Adding point" << point.x << point.y;
		result->vertices.push_back(point);
		// prepare next triangle
		if (side == SIDE_0_DEG)
			triangle_top += R2Vector(0, step * sqrt(3)) * (reversed ? 1 : -1);
		else
			triangle_top = point2;
		reversed = !reversed;
		// find out which side to process next
		value1 = function(triangle_top, arg);
		if (side == SIDE_0_DEG) {
			//qDebug() << "case 0: comparing" << value1 << value2;
			new_side = (compare_signs(value1, value2) ? SIDE_60_DEG : SIDE_120_DEG);
		}
		else if (side == SIDE_60_DEG) {
			point2 = triangle_top - polar_vector(2./3, step, reversed);
			value2 = function(point2, arg);
			//qDebug() << "case 60: comparing" << value1 << value2;
			new_side = (compare_signs(value1, value2) ? SIDE_0_DEG : SIDE_120_DEG);
		} else {
			point2 = triangle_top - polar_vector(1./3, step, reversed);
			value2 = function(point2, arg);
			//qDebug() << "case 120: comparing" << value1 << value2;
			new_side = (compare_signs(value1, value2) ? SIDE_0_DEG : SIDE_60_DEG);
		}
		side = new_side;
	}
	result->rectangle = *rectangle;
}

uint contour_get_width(Contour *contour, double step) {
	return contour->rectangle.width() / step;
}

uint contour_get_height(Contour *contour, double step) {
	return contour->rectangle.height() / step;
}

inline void bitmap_clear(bool *bitmap, uint size) {
	for (uint i = 0; i < size; ++i)
		bitmap[i] = false;
}

void bitmap_fill_from_contour(bool *bitmap, Contour *contour, double step) {
	uint i, j, size = contour->vertices.size();
	R2Point point, nextpoint;
	R2Vector vect;
	double minx = contour->rectangle.getXMin(),
		miny = contour->rectangle.getYMin();
	uint width = contour->rectangle.width() / step,
	    height = contour->rectangle.height() / step;
	
	bitmap_clear(bitmap, width * height);
	for (i = 0; i < size; ++i) {
		point = contour->vertices.at(i);
		nextpoint = contour->vertices.at(i+1 < size ? i+1 : 0);
		vect = nextpoint - point;
		for (j = 0; j < vect.length() / (step/4); ++j) {
			point += vect * (step/4 / vect.length());
			bitmap[uint((point.y - miny) / step) * width
			     + uint((point.x - minx) / step)] = true;
		}
	}
}

void function_process_point(R2Rectangle *rectangle, R2Point origpoint,
int newx, int newy, int *minx, int *miny, double *minres, double step) {
	//qDebug() << "processing point " << newx << newy;
	R2Point newpoint(double(newx)*step + rectangle->getXMin(),
		double(newy)*step + rectangle->getYMin());
	double res = (newpoint-origpoint).length();
	if (*minres < 0 || res < *minres) {
		*minres = res;
		*minx = newx;
		*miny = newy;
	}
}

double function_from_bitmap(R2Point point, bool *bitmap, double step,
R2Rectangle *rectangle) {
	int x = (point.x - rectangle->getXMin()) / step,
	    y = (point.y - rectangle->getYMin()) / step;
	int width = rectangle->width() / step, height = rectangle->height() / step;
	int level = 0, i, j, mini = -1, minj = -1;
	double minres = -1;

	//qDebug() << "our point is" << x << y;
	if (bitmap[y*width+x]) return 0;

    bool inside = false;
    for (i = 0; i < x; ++i)
		if (bitmap[y*width+i])
			inside = true;

	while (minres < 0) {
		j = y - level;
		if (j >= 0 && j <= height)
			for (i = (x >= level ? x-level : 0); i <= (x+level < width ? x+level : width-1); ++i)
				if (bitmap[j * width + i])
					function_process_point(rectangle, point, i, j, &mini, &minj, &minres, step);
				//else qDebug() << "point" << i << j << "is false";
		j = y + level;
		if (j >= 0 && j <= height)
			for (i = (x >= level ? x-level : 0); i <= (x+level < width ? x+level : width-1); ++i)
				if (bitmap[j * width + i])
					function_process_point(rectangle, point, i, j, &mini, &minj, &minres, step);
				//else qDebug() << "point" << i << j << "is false";
		i = x - level;
		if (i >= 0 && i <= width)
			for (j = (y >= level-1 ? y-level+1 : 1); j <= (y+level-1 < height ? y+level-1 : height-1); ++j)
				if (bitmap[j * width + i])
					function_process_point(rectangle, point, i, j, &mini, &minj, &minres, step);
				//else qDebug() << "point" << i << j << "is false";
		i = x + level;
		if (i >= 0 && i <= width)
			for (j = (y >= level-1 ? y-level+1 : 1); j <= (y+level-1 < height ? y+level-1 : height-1); ++j)
				if (bitmap[j * width + i])
					function_process_point(rectangle, point, i, j, &mini, &minj, &minres, step);
				//else qDebug() << "point" << i << j << "is false";
		++level;
	}
	return (inside ? minres : -minres);
}

double function_from_bitmap_advanced(R2Point point, void *arg) {
	Arguments *args = (Arguments *)arg;
	double val1 = function_from_bitmap(point, args->bitmap1, args->step,
		args->rectangle);
	double val2 = function_from_bitmap(point, args->bitmap2, args->step,
		args->rectangle);
	double pos = double(args->position) / 99;
	return val1 * pos + val2 * (1 - pos);
}
