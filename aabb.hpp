#ifndef AABB_H
#define AABB_H

#include "rtweekend.hpp"

class Aabb {
public:
    Interval x, y, z;

    Aabb() {}

    Aabb(const Interval ix, const Interval iy, const Interval iz) : x(ix), y(iy), z(iz) {}

    Aabb(const Point3d& a, const Point3d& b) {
        x = Interval(fmin(a.x(), b.x()), fmax(a.x(), b.x()));
        y = Interval(fmin(a.y(), b.y()), fmax(a.y(), b.y()));
        z = Interval(fmin(a.z(), b.z()), fmax(a.z(), b.z()));
    }

    Aabb(const Aabb& box1, const Aabb& box2) {
        x = Interval(box1.x, box2.x);
        y = Interval(box1.y, box2.y);
        z = Interval(box1.z, box2.z);
    }

    Aabb pad() const {
        double delta = 0.0001;
        Interval new_x = (x.size() >= delta) ? x : x.expand(delta);
        Interval new_y = (y.size() >= delta) ? y : y.expand(delta);
        Interval new_z = (z.size() >= delta) ? z : z.expand(delta);

        return Aabb(new_x, new_y, new_z);
    }

    const Interval& axis(int n) const {
        if (n == 1) {
            return y;
        }
        if (n == 2) {
            return z;
        }
        return x;
    }

    bool hit(const Ray& r, Interval ray_t) const {
        for (int a = 0; a < 3; a++) {
            auto invD = 1 / r.direction()[a];
            auto orig = r.origin()[a];

            auto t0 = (axis(a).min - orig) * invD;
            auto t1 = (axis(a).max - orig) * invD;

            if (invD < 0) std::swap(t0, t1);

            if (t0 > ray_t.min) {
                ray_t.min = t0;
            }
            if (t1 < ray_t.max) {
                ray_t.max = t1;
            }

            if (ray_t.max <= ray_t.min) {
                return false;
            }
        }
        return true;
    }
};

Aabb operator+(const Aabb& bbox, const Vector3d& offset) {
    return Aabb(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

Aabb operator+(const Vector3d& offset, const Aabb& bbox) { return bbox + offset; }

#endif  // AABB_H