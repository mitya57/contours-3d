#ifndef _STRUCT_H
#define _STRUCT_H
#include <climits>  /* For CHAR_BIT */
#include <cmath>    /* For sqrt */
#include <cstddef>  /* For size_t */
#include <cstring>  /* For memset */

template <class CoordType=float> struct Point3D;
template <class CoordType=float> struct Vector3D;
template <class CoordType=float> struct Box;
template <class CoordType=float> struct VoxelArray;

typedef Vector3D<unsigned> Index3D;

template <class CoordType>
struct Point3D {
    CoordType x;
    CoordType y;
    CoordType z;

    Point3D<CoordType>():
        x(0.), y(0.), z(0.) {}
    Point3D<CoordType>(CoordType x, CoordType y, CoordType z):
        x(x), y(y), z(z) {}

    inline Point3D<CoordType> operator + (Vector3D<CoordType> const &that) const {
        return Point3D<CoordType>(x + that.x, y + that.y, z + that.z);
    }
    inline Point3D<CoordType> operator * (CoordType factor) const {
        return Point3D<CoordType>(x * factor, y * factor, z * factor);
    }
    inline Point3D<CoordType> operator / (CoordType factor) const {
        return Point3D<CoordType>(x / factor, y / factor, z / factor);
    }
    inline Vector3D<CoordType> operator - (Point3D<CoordType> const &that) const {
        return Vector3D<CoordType>(x - that.x, y - that.y, z - that.z);
    }
};

template <class CoordType>
struct Vector3D {
    CoordType x;
    CoordType y;
    CoordType z;

    Vector3D<CoordType>():
        x(0.), y(0.), z(0.) {}
    Vector3D<CoordType>(CoordType _x, CoordType _y, CoordType _z):
        x(_x), y(_y), z(_z) {}
    Vector3D<CoordType>(Vector3D<CoordType> const &that):
        x(that.x), y(that.y), z(that.z) {}

    inline CoordType length() const {
        return sqrt(x * x + y * y + z * z);
    }
    inline Vector3D<CoordType> operator * (Index3D const &factors) const {
        return Vector3D<CoordType>(x * factors.x, y * factors.y, z * factors.z);
    }
    inline Vector3D<CoordType> operator * (CoordType factor) const {
        return Vector3D<CoordType>(x * factor, y * factor, z * factor);
    }
    inline Vector3D<CoordType> &operator = (Vector3D<CoordType> const &that) {
        x = that.x;
        y = that.y;
        z = that.z;
        return *this;
    }
    inline Vector3D<CoordType> operator + (Vector3D<CoordType> const &that) const {
        return Vector3D<CoordType>(x + that.x, y + that.y, z + that.z);
    }
};

template <class CoordType>
struct Box {
    Point3D<CoordType> min;
    Point3D<CoordType> max;

    Box<CoordType>(Point3D<CoordType> min, Point3D<CoordType> max):
        min(min), max(max) {}

    inline bool contains(Point3D<CoordType> point) const {
        return (point.x > min.x && point.y > min.y && point.z > min.z &&
                point.x < max.x && point.y < max.y && point.z < max.z);
    }
};

struct BitSet {
    signed char *data;

    BitSet(size_t length) {
        data = new signed char[length / CHAR_BIT + 1];
        memset(data, 0, length / CHAR_BIT + 1);
    }
    ~BitSet() {
        delete[] data;
    }

    inline void setValue(size_t i, bool value) const {
        if ((*this)[i] != value) {
            data[i / CHAR_BIT] ^= (1 << (i % CHAR_BIT));
        }
    }
    inline bool operator[] (size_t i) const {
        return (data[i / CHAR_BIT] >> (i % CHAR_BIT)) & 1;
    }
};

template <class CoordType>
struct VoxelArray {
    BitSet voxels;
    Point3D<CoordType> start;
    Vector3D<CoordType> voxelSize;
    Index3D size;

    VoxelArray<CoordType>(Index3D _size):
        voxels(_size.x * _size.y * _size.z), size(_size) {}

    inline Box<CoordType> getBox() const {
        return Box<CoordType>(start, start + voxelSize * size);
    }
    inline bool verifyIndex(Index3D const &index) const {
        return (index.x > 0 && index.x < size.x &&
                index.y > 0 && index.y < size.y &&
                index.z > 0 && index.z < size.z);
    }
    inline unsigned num(Index3D const &index) const {
        return index.z * size.x * size.y + index.y * size.x + index.z;
    }
    inline bool operator [] (Index3D const &index) const {
        return voxels[num(index)];
    }
    inline void setVoxel(Index3D const &index, bool value) {
        voxels.setValue(num(index), value);
    }
    inline Point3D<CoordType> getVoxelCenter(Index3D const &index) const {
        return start + (voxelSize * .5) + (voxelSize * index);
    }
};

#endif /* _STRUCT_H */
