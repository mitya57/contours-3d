#include "struct.h"

template <class CoordType>
void processPoint(Point3D<CoordType>     point,
                  VoxelArray<CoordType> *array,
                  Index3D                index,
                  CoordType             *currentLength)
{
    if (!array[index] || !array->verifyIndex(index)) {
        return;
    }
    CoordType length = (point - array->getVoxelCenter(index)).length();
    if (*currentLength < 0 || length < *currentLength) {
        *currentLength = currentLength;
    }
}

template <class CoordType>
CoordType getDistance(Point3D<CoordType>     point,
                      VoxelArray<CoordType> *array)
{
    CoordType currentLength = -1;
    int x, y, z;

    Index3D initialIndex(
        (point.x - array->start.x) / array->voxelSize.x,
        (point.y - array->start.y) / array->voxelSize.y,
        (point.z - array->start.z) / array->voxelSize.z
    );
    if (*array[initialIndex]) {
        return 0;
    }

    for (unsigned layerInd = 1;; ++layerInd) {
        /* min z and max z */
        for (x = initialIndex.x - layerInd; x <= initialIndex.x + layerInd; ++x) {
            for (y = initialIndex.y - layerInd; y <= initialIndex.y + layerInd; ++y) {
                processPoint(point, array,
                             Index3D(x, y, initialIndex.z - layerInd),
                             &currentLength);
                processPoint(point, array,
                             Index3D(x, y, initialIndex.z + layerInd),
                             &currentLength);
            }
        }
        /* min y and max y */
        for (x = initialIndex.x - layerInd; x <= initialIndex.x + layerInd; ++x) {
            for (z = initialIndex.z - layerInd + 1; z < initialIndex.z + layerInd; ++z) {
                processPoint(point, array,
                             Index3D(x, initialIndex.y - layerInd, z),
                             &currentLength);
                processPoint(point, array,
                             Index3D(x, initialIndex.y + layerInd, z),
                             &currentLength);
            }
        }
        /* min x and max x */
        for (y = initialIndex.y - layerInd + 1; y < initialIndex.y + layerInd; ++y) {
            for (z = initialIndex.z - layerInd + 1; z < initialIndex.z + layerInd; ++z) {
                processPoint(point, array,
                             Index3D(initialIndex.x - layerInd, y, z),
                             &currentLength);
                processPoint(point, array,
                             Index3D(initialIndex.x + layerInd, y, z),
                             &currentLength);
            }
        }
        if (currentLength > 0) {
            return currentLength;
        }
    }

    return currentLength;
}
