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
    double u;
    double v;
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

class Translate : public Hittable {
public:
    Translate(shared_ptr<Hittable> object, const Vector3d& offset)
        : object(object), offset(offset) {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        Ray offset_r(r.origin() - offset, r.direction(), r.time());

        if (!object->hit(offset_r, ray_t, rec)) {
            return false;
        }

        rec.p += offset;

        return true;
    }

    Aabb bounding_box() const override { return bbox; }

private:
    shared_ptr<Hittable> object;
    Vector3d offset;
    Aabb bbox;
};

class RotateY : public Hittable {
public:
    RotateY(shared_ptr<Hittable> object, double angle) : object(object) {
        auto radians = deg_to_rad(angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        bbox = object->bounding_box();

        Point3d min(infinity, infinity, infinity);
        Point3d max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

                    auto newx = cos_theta * x + sin_theta * z;
                    auto newz = -sin_theta * x + cos_theta * z;

                    min = Vector3d(fmin(min.x(), newx), fmin(min.y(), y), fmin(min.z(), newz));
                    max = Vector3d(fmax(max.x(), newx), fmax(max.y(), y), fmax(max.z(), newz));
                }
            }
        }

        bbox = Aabb(min, max);
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        auto origin =
            Point3d(cos_theta * r.origin().x() - sin_theta * r.origin().z(), r.origin().y(),
                    sin_theta * r.origin().x() + cos_theta * r.origin().z());
        auto dir = Vector3d(cos_theta * r.direction().x() - sin_theta * r.direction().z(),
                            r.direction().y(),
                            sin_theta * r.direction().x() + cos_theta * r.direction().z());

        Ray rotated_r(origin, dir, r.time());

        if (!object->hit(rotated_r, ray_t, rec)) {
            return false;
        }

        auto p = Point3d(cos_theta * rec.p.x() + sin_theta * rec.p.z(), rec.p.y(),
                         -sin_theta * rec.p.x() + cos_theta * rec.p.z());
        auto normal =
            Vector3d(cos_theta * rec.normal.x() + sin_theta * rec.normal.z(), rec.normal.y(),
                     -sin_theta * rec.normal.x() + cos_theta * rec.normal.z());

        rec.p = p;
        rec.normal = normal;

        return true;
    }

    Aabb bounding_box() const override { return bbox; }

private:
    shared_ptr<Hittable> object;
    double sin_theta;
    double cos_theta;
    Aabb bbox;
};

#endif  // HITTABLE_H