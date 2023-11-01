#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.hpp"
#include "rtweekend.hpp"

class Material;

class HitRecord {
public:
    Point3d p;
    Vector3d normal;
    shared_ptr<Material> mat;
    double t;
    bool front_face;

    void set_face_normal(const Ray& r, const Vector3d& outward_normal) {
        // Sets the HitRecord normal vector
        // NOTE: 'outward_normal' is assumed to be a unit vector
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const = 0;
    virtual Aabb bounding_box() const = 0;
};

#endif  // HITTABLE_H