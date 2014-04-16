#include "struct.h"
#include <set>

#define D_EQUAL(d1, d2) (fabs(d1 - d2) < 1e-10)

template <class CoordType>
void processPoint(Point3D<CoordType>           point,
                  VoxelArray<CoordType> const &array,
                  Index3D                      index,
                  CoordType                   &currentLength)
{
    if (!array[index] || !array.verifyIndex(index)) {
        return;
    }
    CoordType length = (point - array.getVoxelCenter(index)).length();
    if (currentLength < 0 || length < currentLength) {
        currentLength = currentLength;
    }
}

template <class CoordType>
CoordType getDistance(Point3D<CoordType>     point,
                      VoxelArray<CoordType> &array)
{
    CoordType currentLength = -1;
    int x, y, z;

    Index3D initialIndex(
        (point.x - array.start.x) / array.voxelSize.x,
        (point.y - array.start.y) / array.voxelSize.y,
        (point.z - array.start.z) / array.voxelSize.z
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

template <class CoordType, VoxelArray<CoordType> const &array, CoordType *distArray>
struct IndexCompare3D {
    bool operator() (Index3D const &ind1, Index3D const &ind2) const {
        unsigned num1 = array.num(ind1);
        unsigned num2 = array.num(ind2);
        /* First, order by distance */
        if (!D_EQUAL(distArray[num1], distArray[num2])) {
            return distArray[num1] > distArray[num2];
        }
        /* If distances are equal, order by j */
        return num1 > num2;
    }
};

template <class CoordType>
CoordType fillDistanceArray(VoxelArray<CoordType> const &array,
                            CoordType                   *distArray)
/* We expect that distArray points to a memory of the same
 * size as array.voxels. */
{
    Index3D neighbour;
    Index3D &currentInd;
    std::set<Index3D, IndexCompare3D<CoordType, array, distArray> > processSet;
    CoordType dist, currentDist;
    unsigned num;

    for (unsigned x = 0; x < array.size.x; ++x) {
        for (unsigned y = 0; y < array.size.y; ++y) {
            for (unsigned z = 0; z < array.size.z; ++z) {
                currentInd = Index3D(x, y, z);
                num = array.num(currentInd);
                if (array.voxels[num]) {
                    distArray[num] = 0;
                    processSet.insert(currentInd);
                }
            }
        }
    }

    while (!processSet.empty()) {
        /* States:
         * - processed: distArray[array.num(index)] != -1 && not in the set
         * - toProcess: in the set
         * - null state: distArray[array.num(index)] == -1 && not in the set
         */

        /* 1) currentInd := (one in the set with lowest diff)
         * 2) update distances for all neighbours that are not already
         *    processed, insert them to the set if not yet there,
         *    and move them if already there
         * 3) remove currentInd from the set
         */

        currentInd = processSet.begin();

        for (unsigned char j = 0; j < 27; ++j) {
            Vector3D<signed char> diff(
                j % 3,   (j % 9) / 3,   j / 9
            );
            if (
                j == 13 || /* equal to currentInd */
                !(currentInd.x || diff.x) ||
                !(currentInd.y || diff.y) ||
                !(currentInd.z || diff.z)
            ) {
                continue;
            }
            neighbour.x = currentInd.x + diff.x - 1;
            neighbour.y = currentInd.y + diff.y - 1;
            neighbour.z = currentInd.z + diff.z - 1;
            if (
                neighbour.x >= array.size.x ||
                neighbour.y >= array.size.y ||
                neighbour.z >= array.size.z
            ) {
                continue;
            }

            dist = distArray[array.num(currentInd)] +
                   Vector3D<CoordType>(diff.x - 1, diff.y - 1, diff.z - 1).length();
            num = array.num(neighbour);
            if (distArray[num] < -.5) {
                distArray[num] = dist;
                processSet.insert(neighbour);
            } else if (distArray[num] > dist) {
                distArray[num] = dist;
            }
        }
        processSet.erase(currentInd);
    }
}
