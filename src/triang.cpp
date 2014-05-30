#include <algorithm>
#include "triang.h"

Index3D getVertexOffset(char vnumber) {
    return Index3D(
        vnumber & 1,
        bool(vnumber & 2),
        bool(vnumber & 4)
    )
}

template <class CoordType, Point3D<CoordType> center, CoordType radius>
CoordType sphereFunction(Point3D<CoordType> point) {
    return radius - (point - center).length();
}

float (*mySphereFunction) (Point3D<float>) =
    sphereFunction<float, Point3Df(.5f, .5f, .5f), .4f>

void getTetrahedByIndex(char index, char *tnumbers) {
    tnumbers[0] = 0;
    tnumbers[1] = 7;
    tnumbers[2] = 1 << (2 - (index % 3));
    tnumbers[3] = tnumbers[2] + (1 << (index >> 1));
}

struct IndexPair3D {
    Index3D i1;
    Index3D i2;

    IndexPair3D(Index3D _i1, Index3D _i2):
        i1(_i1), i2(_i2) {}

    bool operator == (IndexPair3D const &that) {
        return i1 == that.i1 && i2 == that.i2;
    }
}

template <class Type>
int findInVector(std::vector<Type> const &vect, Type const &element) {
    for (unsigned i = 0; i < vect.size(); ++i) {
        if (vect[i] == element) {
            return i;
        }
    }
    return -1;
}

template <class Type>
int appendToVector(std::vector<Type> &vect, Type const &element) {
    int found = findInVector<Type>(vect, element);
    if (found >= 0) {
        return found;
    }
    vector.push_back(element);
    return vector.size() - 1;
}

int addIndexPair(std::vector<IndexPair3D> &, Type const) = appendToVector<IndexPair3D>;

int fillVectors(std::vector<Point3Df> points;
                std::vector<Triangle3Df> triangles) {
    const unsigned size = 20;
    const float rsize = .05f;

    VoxelArray<float> voxelArray(Index3D(size, size, size));
    voxelArray.start = Point3Df(0.f, 0.f, 0.f);
    voxelArray.voxelSize = Vector3Df(rsize, rsize, rsize);

    float *distArray = new float[voxelArray.elementsCount];
    unsigned i, j, k, l;
    unsigned negative;
    char tnumbers[4];
    float fvalues[4];

    std::vector<IndexPair3D> indexPairs;

    /* FIXME: fill voxelArray and then use fillDistanceArray here,
     *        instead of filling the distance array manually.
     */

    for (i = 0; i < voxelArray.elementsCount; ++i) {
        distArray[i] = mySphereFunction(
            voxelArray.getVoxelCenter(voxelArray.index3d(i)));
    }

    for (i = 0; i < voxelArray.elementsCount; ++i) {
        Index3D ind3d = voxelArray.index3d(i);
        if (ind3d.x == size - 1 || ind3d.y == size - 1 || ind3d.z == size - 1) {
            continue;
        }
        for (j = 0; j < 6; ++j) {
            getTetrahedByIndex(j, tnumbers);
            for (k = 0; k < 4; ++k) {
                fvalues[k] = distArray[ind3d + getVertexOffset(tnumbers[k])];
            }
            negative = 0;
            for (k = 0; k < 4; ++k) {
                for (l = k + 1; l < 4; ++l) {
                    if (fvalues[l] < fvalues[k]) {
                        std::swap(tnumbers[k], tnumbers[l]);
                        std::swap(fvalues[k], fvalues[l]);
                    }
                }
                if (fvalues[k] < 0) {
                    negative = k + 1;
                }
            }
            /* FIXME: set normals correctly */
            if (negative == 1) {
                /* Triangle(01, 02, 03) */
                triangles.push_back(
                    addIndexPair(indexPairs, IndexPair3D(tnumbers[0], tnumbers[1])),
                    addIndexPair(indexPairs, IndexPair3D(tnumbers[0], tnumbers[2])),
                    addIndexPair(indexPairs, IndexPair3D(tnumbers[0], tnumbers[3]))
                );
            } else if (negative == 2) {
                /* Rectangle(02, 12, 13), Rectangle(02, 03, 13) */
                triangles.push_back(
                    addIndexPair(indexPairs, IndexPair3D(tnumbers[0], tnumbers[2])),
                    addIndexPair(indexPairs, IndexPair3D(tnumbers[1], tnumbers[2])),
                    addIndexPair(indexPairs, IndexPair3D(tnumbers[1], tnumbers[3]))
                );
                triangles.push_back(
                    addIndexPair(indexPairs, IndexPair3D(tnumbers[0], tnumbers[2])),
                    addIndexPair(indexPairs, IndexPair3D(tnumbers[0], tnumbers[3])),
                    addIndexPair(indexPairs, IndexPair3D(tnumbers[1], tnumbers[3]))
                );
            }
        }
    }

    IndexPair3D &pair;
    for (i = 0; i < indexPairs.size(); ++i) {
        pair = indexPairs[i];
        float val1 = distArray[ind3d + getVertexOffset(pair.i1)];
        float val2 = distArray[ind3d + getVertexOffset(pair.i2)];
        Point3Df p1 = Point3Df(rsize, rsize, rsize) * pair.i1;
        Point3Df p2 = Point3Df(rsize, rsize, rsize) * pair.i2;
        points.push_back((p1 * val2 - p2 * val1) / (val2 - val1));
    }

    delete[] distArray;
}
