#ifndef QUAD_H
#define QUAD_H

#include "hittable.hpp"
#include "rtweekend.hpp"

class Quad : public Hittable {
public:
    Quad(const Point3d& q, const Vector3d& u, const Vector3d& v, shared_ptr<Material> mat)
        : q(q), u(u), v(v), mat(mat) {
        auto n = cross(u, v);
        normal = unit_vector(n);
        d = dot(normal, q);
        w = n / dot(n, n);

        set_bounding_box();
    }

    virtual void set_bounding_box() { bbox = Aabb(q, q + u + v).pad(); }

    Aabb bounding_box() const override { return bbox; }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        auto denom = dot(normal, r.direction());

        if (fabs(denom) < 1e-8) {
            return false;
        }

        auto t = (d - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t)) {
            return false;
        }

        auto intersection = r.at(t);
        Vector3d planar_hitpoint = intersection - q;
        auto alpha = dot(w, cross(planar_hitpoint, v));
        auto beta = dot(w, cross(u, planar_hitpoint));

        if (!is_interior(alpha, beta, rec)) {
            return false;
        }

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, HitRecord& rec) const {
        if ((a < 0) || (1 < a) || (b < 0) || (1 < b)) {
            return false;
        }

        rec.u = a;
        rec.v = b;
        return true;
    }

private:
    Point3d q;
    Vector3d u, v;
    shared_ptr<Material> mat;
    Aabb bbox;
    Vector3d normal;
    double d;
    Vector3d w;
};

#endif  // QUAD_H