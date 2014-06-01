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

template <class CoordType>
void fillDistanceArray(VoxelArray<CoordType> const &array,
                       CoordType                   *distArray,
                       bool                         reverse)
/* We expect that distArray points to a memory of the same
 * size as array.voxels. */
{
    Index3D neighbour;
    std::set<SortableIndex3D<CoordType>, SortableIndexCompare3D<CoordType> > processSet;
    CoordType dist;
    unsigned num;

    for (unsigned num = 0; num < array.elementsCount; ++num) {
        Index3D currentInd = array.index3d(num);
        if (array.voxels[num] != reverse) {
            distArray[num] = 0;
            processSet.insert(SortableIndex3D<CoordType>(
                              currentInd, array, distArray));
        } else {
            distArray[num] = -1;
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
}

void (*fillDistanceArray_d) (VoxelArray<double> const &, double *, bool) = fillDistanceArray<double>;
void (*fillDistanceArray_f) (VoxelArray<float> const &, float *, bool) = fillDistanceArray<float>;
