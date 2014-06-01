#include <set>
#include "distarray.h"

template <class CoordType>
struct SortableIndex3D: Index3D {
    /* Some additional fields that are needed for sorting */
    SortableIndex3D<CoordType> (Index3D const &_index3d,
                                VoxelArray<CoordType> const &_array,
                                CoordType const *_distArray):
        Index3D(_index3d), array(_array), distArray(_distArray) {}

    VoxelArray<CoordType> const &array;
    CoordType const *distArray;
};

template <class CoordType>
struct SortableIndexCompare3D {
    bool operator() (SortableIndex3D<CoordType> const &ind1,
                     SortableIndex3D<CoordType> const &ind2) const
    {
        unsigned num1 = ind1.array.num(ind1);
        unsigned num2 = ind2.array.num(ind2);
        /* First, order by distance */
        if (!dEqual(ind1.distArray[num1], ind2.distArray[num2])) {
            return ind1.distArray[num1] > ind2.distArray[num2];
        }
        /* If distances are equal, order by j */
        return num1 > num2;
    }
};

bool fillNeighbour(Index3D const         &size,
                   Index3D const         &currentInd,
                   signed char            j,
                   Index3D               &neighbour,
                   Vector3D<signed char> &diff) {
    diff = Vector3D<signed char>(
         j % 3,   (j % 9) / 3,   j / 9
    );
    if (
        j == 13 || /* equal to currentInd */
        !(currentInd.x || diff.x) ||
        !(currentInd.y || diff.y) ||
        !(currentInd.z || diff.z)
    ) {
        return false;
    }
    neighbour.x = currentInd.x + diff.x - 1;
    neighbour.y = currentInd.y + diff.y - 1;
    neighbour.z = currentInd.z + diff.z - 1;
    if (
        neighbour.x >= size.x ||
        neighbour.y >= size.y ||
        neighbour.z >= size.z
    ) {
        return false;
    }
    return true;
}

template <class CoordType>
void fillDistanceArray(VoxelArray<CoordType> const &array,
                       CoordType                   *distArray)
/* We expect that distArray points to a memory of the same
 * size as array.voxels. */
{
    Index3D neighbour;
    Vector3D<signed char> diff;
    std::set<SortableIndex3D<CoordType>, SortableIndexCompare3D<CoordType> > processSet;
    CoordType dist;
    unsigned num;

    for (unsigned num = 0; num < array.elementsCount; ++num) {
        Index3D currentInd = array.index3d(num);
        distArray[num] = -1;
        if (array.voxels[num]) {
            bool foundNotInArray = false;
            for (unsigned char j = 0; j < 27; ++j) {
                if (fillNeighbour(array.size, currentInd, j, neighbour, diff)) {
                    if (!array.voxels[array.num(neighbour)]) {
                        foundNotInArray = true;
                    }
                }
            }
            if (foundNotInArray) {
                distArray[num] = 0;
                processSet.insert(SortableIndex3D<CoordType>(
                                  currentInd, array, distArray));
            }
        }
    }

    while (!processSet.empty()) {
        /* States:
         * - processed: distArray[array.num(index)] != -1 && not in the set
         * - toProcess: in the set
         * - null state: distArray[array.num(index)] == -1 && not in the set
         *
         * Procedure:
         * 1) currentInd := (one in the set with lowest diff)
         * 2) update distances for all neighbours that are not already
         *    processed, insert them to the set if not yet there,
         *    and move them if already there
         * 3) remove currentInd from the set
         */

        SortableIndex3D<CoordType> const currentInd = *processSet.begin();

        for (unsigned char j = 0; j < 27; ++j) {
            if (!fillNeighbour(array.size, currentInd, j, neighbour, diff)) {
                continue;
            }
            dist = distArray[array.num(currentInd)] + Vector3D<CoordType>(
                array.voxelSize.x * (diff.x - 1),
                array.voxelSize.y * (diff.y - 1),
                array.voxelSize.z * (diff.z - 1)
            ).length();
            num = array.num(neighbour);
            if (distArray[num] < -.5) {
                distArray[num] = dist;
                processSet.insert(SortableIndex3D<CoordType>(neighbour, array, distArray));
            } else if (distArray[num] > dist) {
                distArray[num] = dist;
            }
        }
        processSet.erase(currentInd);
    }

    for (unsigned num = 0; num < array.elementsCount; ++num) {
        if (array.voxels[num]) {
            distArray[num] = -distArray[num];
        }
    }
}

void (*fillDistanceArray_d) (VoxelArray<double> const &, double *) = fillDistanceArray<double>;
void (*fillDistanceArray_f) (VoxelArray<float> const &, float *) = fillDistanceArray<float>;
