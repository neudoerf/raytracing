#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.hpp"
#include "hittable_list.hpp"
#include "rtweekend.hpp"
#include "texture.hpp"

class HitRecord;

class Material {
public:
    virtual ~Material() = default;

    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                         Ray& scattered) const = 0;

    virtual Color emitted(double u, double v, const Point3d& p) const { return Color(0, 0, 0); }
};

class Lambertian : public Material {
public:
    Lambertian(const Color& a) : albedo(make_shared<SolidColor>(a)) {}

    Lambertian(shared_ptr<Texture> a) : albedo(a) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // catch degenerate scatter direction
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }
        scattered = Ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    shared_ptr<Texture> albedo;
};

class Metal : public Material {
public:
    Metal(const Color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override {
        Vector3d reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz * random_unit_vector(), r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    Color albedo;
    double fuzz;
};

class Dielectric : public Material {
public:
    Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override {
        attenuation = Color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        Vector3d unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vector3d direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
            direction = reflect(unit_direction, rec.normal);
        } else {
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        scattered = Ray(rec.p, direction, r_in.time());
        return true;
    }

private:
    double ir;

    static double reflectance(double cosine, double ref_idx) {
        // use Schlick's approximation for reflectance
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

class DiffuseLight : public Material {
public:
    DiffuseLight(shared_ptr<Texture> a) : emit(a) {}

    DiffuseLight(Color c) : emit(make_shared<SolidColor>(c)) {}

    bool scatter(const Ray& r, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override {
        return false;
    }

    Color emitted(double u, double v, const Point3d& p) const override {
        return emit->value(u, v, p);
    }

private:
    shared_ptr<Texture> emit;
};

class Isotropic : public Material {
public:
    Isotropic(Color c) : albedo(make_shared<SolidColor>(c)) {}

    Isotropic(shared_ptr<Texture> a) : albedo(a) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override {
        scattered = Ray(rec.p, random_unit_vector(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    shared_ptr<Texture> albedo;
};

#endif  // MATERIAL_H