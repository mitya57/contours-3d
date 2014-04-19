#ifndef _FUNCTION_H
#define _FUNCTION_H
#include "struct.h"

template <class CoordType>
CoordType getDistance(Point3D<CoordType>     point,
                      VoxelArray<CoordType> &array);

template <class CoordType>
CoordType fillDistanceArray(VoxelArray<CoordType> const &array,
                            CoordType                   *distArray);

#endif /* _FUNCTION_H */
