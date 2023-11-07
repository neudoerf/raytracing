#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.hpp"
#include "material.hpp"
#include "rtweekend.hpp"
#include "texture.hpp"

class ConstantMedium : public Hittable {
public:
    ConstantMedium(shared_ptr<Hittable> b, double d, shared_ptr<Texture> a)
        : boundary(b), neg_inv_density(-1 / d), phase_func(make_shared<Isotropic>(a)) {}

    ConstantMedium(shared_ptr<Hittable> b, double d, Color c)
        : boundary(b), neg_inv_density(-1 / d), phase_func(make_shared<Isotropic>(c)) {}

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        const bool enableDebug = false;
        const bool debugging = enableDebug && random_double() < 0.00001;

        HitRecord rec1, rec2;
        if (!boundary->hit(r, Interval::universe, rec1)) {
            return false;
        }

        if (!boundary->hit(r, Interval(rec1.t + 0.0001, infinity), rec2)) {
            return false;
        }

        if (debugging) {
            std::clog << "\nray_tmin=" << rec1.t << ", ray_tmax=" << rec2.t << "\n";
        }

        if (rec1.t < ray_t.min) {
            rec1.t = ray_t.min;
        }
        if (rec2.t > ray_t.max) {
            rec2.t = ray_t.max;
        }
        if (rec1.t >= rec2.t) {
            return false;
        }
        if (rec1.t < 0) {
            rec1.t = 0;
        }

        auto ray_length = r.direction().length();
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = neg_inv_density * log(random_double());

        if (hit_distance > distance_inside_boundary) {
            return false;
        }

        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        if (debugging) {
            std::clog << "hit_distance=" << hit_distance << "\n"
                      << "rec.t=" << rec.t << "\n"
                      << "rec.p=" << rec.p << "\n";
        }

        rec.normal = Vector3d(1, 0, 0);  // arbitrary
        rec.front_face = true;           // arbitrary
        rec.mat = phase_func;

        return true;
    }

    Aabb bounding_box() const override { return boundary->bounding_box(); }

private:
    shared_ptr<Hittable> boundary;
    double neg_inv_density;
    shared_ptr<Material> phase_func;
};

#endif  // CONSTANT_MEDIUM_H