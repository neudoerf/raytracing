#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.hpp"
#include "vector3d.hpp"

class Sphere : public Hittable {
public:
    Sphere(Point3d _center, double _radius, shared_ptr<Material> _material)
        : center1(_center), radius(_radius), mat(_material), is_moving(false) {
        auto rvec = Vector3d(radius, radius, radius);
        bbox = Aabb(center1 + rvec, center1 - rvec);
    }

    Sphere(Point3d _center1, Point3d _center2, double _radius, shared_ptr<Material> _material)
        : center1(_center1), radius(_radius), mat(_material), is_moving(true) {
        auto rvec = Vector3d(radius, radius, radius);
        Aabb box1(_center1 - rvec, _center1 + rvec);
        Aabb box2(_center2 - rvec, _center2 + rvec);
        bbox = Aabb(box1, box2);

        center_vec = _center2 - _center1;
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        Point3d center = is_moving ? sphere_center(r.time()) : center1;
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

    Aabb bounding_box() const override { return bbox; }

private:
    Point3d center1;
    double radius;
    shared_ptr<Material> mat;
    bool is_moving;
    Vector3d center_vec;
    Aabb bbox;

    Point3d sphere_center(double time) const { return center1 + time * center_vec; }
};

#endif  // SPHERE_H