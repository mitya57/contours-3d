#include "R2Graph.h"

#define MAX(x, y) (x > y ? x : y)

#ifdef NO_QT
#include <vector>
typedef std::vector<R2Point> VerticesVector;
#else
#include <QVector>
typedef QVector<R2Point> VerticesVector;
#endif

typedef unsigned int uint;

struct Contour {
	VerticesVector vertices;
	R2Rectangle rectangle;
	//R2Point centroid;
	//R2Point center;
};

struct Arguments {
	bool *bitmap1;
	bool *bitmap2;
	double step;
	R2Rectangle *rectangle;
	int position;
};

struct PolygonArguments {
	uint size1;
	uint size2;
	double radius1;
	double radius2;
	int position;
};

enum Side {
	SIDE_0_DEG,
	SIDE_60_DEG,
	SIDE_120_DEG
};

double f_polygons(R2Point point, void *arg);

void contour_new_from_function(Contour *result, double function(R2Point, void *),
void *arg, double step, R2Rectangle *rectangle);

#define contour_from_function contour_new_from_function

void bitmap_fill_from_contour(bool *bitmap, Contour *contour, double step);
double function_from_bitmap_advanced(R2Point point, void *arg);
