#ifndef __DISTARRAY_H
#define __DISTARRAY_H
#include "struct.h"

template <class CoordType>
void fillDistanceArray(VoxelArray<CoordType> const &array,
                       CoordType                   *distArray);

template <class CoordType>
inline bool dEqual(CoordType d1, CoordType d2) {
    return fabs(d1 - d2) < 1e-10;
}

#endif /* __DISTARRAY_H */
