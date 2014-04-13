#ifndef _STRUCT_H
#define _STRUCT_H
#include <cmath>

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

    Vector3D<CoordType>(CoordType x, CoordType y, CoordType z):
        x(x), y(y), z(z) {}

    inline CoordType length() const {
        return sqrt(x * x + y * y + z * z);
    }
    inline Vector3D<CoordType> operator * (Index3D const &factors) const {
        return Vector3D<CoordType>(x * factors.x, y * factors.y, z * factors.z);
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

template <class CoordType>
struct VoxelArray {
    bool *voxels;
    Point3D<CoordType> start;
    Vector3D<CoordType> voxelSize;
    Index3D size;

    inline Box<CoordType> getBox() const {
        return Box<CoordType>(start, start + voxelSize * size);
    }
    inline bool verifyIndex(Index3D const &index) const {
        return (index.x > 0 && index.x < size.x &&
                index.y > 0 && index.y < size.y &&
                index.z > 0 && index.z < size.z);
    }
    inline bool &operator [] (Index3D const &index) {
        return voxels[index.z * size.x * size.y + index.y * size.x + index.z];
    }
    inline CoordType getVoxelCenter (Index3D const &index) const {
        return start + (voxelSize / 2) + (voxelSize * index);
    }
};

#endif /* _STRUCT_H */
