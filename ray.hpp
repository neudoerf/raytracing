#ifndef RAY_H
#define RAY_H

#include "vector3d.hpp"

class Ray {
private:
    Point3d orig;
    Vector3d dir;
    double tm;

public:
    Ray() {}

    Ray(const Point3d& origin, const Vector3d& dir, const double time)
        : orig(origin), dir(dir), tm(time) {}

    Point3d origin() const { return orig; }

    Vector3d direction() const { return dir; }

    double time() const { return tm; }

    Point3d at(double t) const { return orig + t * dir; }
};

#endif  // RAY_H