#ifndef __TRIANG_H
#define __TRIANG_H

#include <vector>
#include "struct.h"

template <class CoordType>
struct Triangle3D {
    typedef std::vector<Point3D<CoordType> > PointArray3D;

    PointArray3D &pointArray;
    unsigned ind[3];

    Triangle3D(PointArray3D &_pointArray, unsigned ind1, unsigned ind2, unsigned ind3):
        pointArray(_pointArray)
    {
        ind[0] = ind1;
        ind[1] = ind2;
        ind[2] = ind3;
    }

    Triangle3D(PointArray3D &_pointArray, unsigned _ind[3]):
        pointArray(_pointArray)
    {
        memcpy(ind, _ind, 3 * sizeof(unsigned));
    }
};

#endif /* __TRIANG_H */
