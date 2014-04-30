#ifndef __DIST_H
#define __DIST_H
#include "struct.h"

template <class CoordType>
CoordType getDistance(Point3D<CoordType>     point,
                      VoxelArray<CoordType> &array);

template <class CoordType>
void fillDistanceArray(VoxelArray<CoordType> const &array,
                       CoordType                   *distArray);

#endif /* __DIST_H */
