#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.hpp"
#include "vector3d.hpp"

class Sphere : public Hittable {
public:
    Sphere(Point3d _center, double _radius, shared_ptr<Material> _material)
        : center(_center), radius(_radius), mat(_material) {}

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        Vector3d oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius * radius;
        auto discriminant = half_b * half_b - a * c;

        if (discriminant < 0) {
            return false;
        }
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        Vector3d outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

private:
    Point3d center;
    double radius;
    shared_ptr<Material> mat;
};

#endif  // SPHERE_H