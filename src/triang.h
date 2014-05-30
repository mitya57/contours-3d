#ifndef __TRIANG_H
#define __TRIANG_H

#include <vector>
#include "struct.h"

template <class CoordType>
struct Triangle3D {
    unsigned ind[3];
    Vector3D<CoordType> normal;

    Triangle3D(unsigned ind1, unsigned ind2, unsigned ind3,
               CoordType nrmX = 0, CoordType nrmY = 0, CoordType nrmZ = 0):
        normal(nrmX, nrmY, nrmZ)
    {
        ind[0] = ind1;
        ind[1] = ind2;
        ind[2] = ind3;
    }

    Triangle3D(unsigned _ind[3], Vector3D<CoordType> _normal):
        normal(_normal)
    {
        memcpy(ind, _ind, 3 * sizeof(unsigned));
    }
};

typedef Triangle3D<double> Triangle3Dd;
typedef Triangle3D<float> Triangle3Df;

void fillVectors(std::vector<Point3Df> points,
                 std::vector<Triangle3Df> triangles);

#endif /* __TRIANG_H */
