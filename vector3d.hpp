#ifndef VECTOR_3D_H
#define VECTOR_3D_H

#include <cmath>
#include <iostream>

using std::sqrt;

class Vector3d {
private:
    double e[3];

public:
    Vector3d() : e{0, 0, 0} {}
    Vector3d(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    Vector3d operator-() const { return Vector3d(-e[0], -e[1], -e[2]); }

    Vector3d& operator+=(const Vector3d& v) {
        e[0] += v.x();
        e[1] += v.y();
        e[2] += v.z();
        return *this;
    }

    Vector3d& operator*=(const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vector3d& operator/=(const double t) { return *this *= 1 / t; }

    double length() const { return sqrt(length_squared()); }

    double length_squared() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }

    double near_zero() const {
        auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

    static Vector3d random() { return Vector3d(random_double(), random_double(), random_double()); }

    static Vector3d random(double min, double max) {
        return Vector3d(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

using Point3d = Vector3d;

inline std::ostream& operator<<(std::ostream& out, const Vector3d& v) {
    return out << v.x() << " " << v.y() << " " << v.z();
}

inline Vector3d operator+(const Vector3d& left, const Vector3d& right) {
    return Vector3d(left.x() + right.x(), left.y() + right.y(), left.z() + right.z());
}

inline Vector3d operator-(const Vector3d& left, const Vector3d& right) {
    return Vector3d(left.x() - right.x(), left.y() - right.y(), left.z() - right.z());
}

inline Vector3d operator*(const Vector3d& left, const Vector3d& right) {
    return Vector3d(left.x() * right.x(), left.y() * right.y(), left.z() * right.z());
}

inline Vector3d operator*(const double left, const Vector3d& right) {
    return Vector3d(left * right.x(), left * right.y(), left * right.z());
}

inline Vector3d operator*(const Vector3d& left, const double right) { return right * left; }

inline Vector3d operator/(const Vector3d& left, const double right) { return left * (1 / right); }

inline double dot(const Vector3d& left, const Vector3d& right) {
    return left.x() * right.x() + left.y() * right.y() + left.z() * right.z();
}

inline Vector3d cross(const Vector3d& left, const Vector3d& right) {
    return Vector3d(left.y() * right.z() - left.z() * right.y(),
                    left.z() * right.x() - left.x() * right.z(),
                    left.x() * right.y() - left.y() * right.x());
}

inline Vector3d unit_vector(const Vector3d& v) { return v / v.length(); }

inline Vector3d random_in_unit_disk() {
    while (true) {
        auto p = Vector3d(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() < 1) {
            return p;
        }
    }
}

inline Vector3d random_in_unit_sphere() {
    while (true) {
        auto p = Vector3d::random(-1, 1);
        if (p.length_squared() < 1) {
            return p;
        }
    }
}

inline Vector3d random_unit_vector() { return unit_vector(random_in_unit_sphere()); }

inline Vector3d random_in_hemisphere(const Vector3d& normal) {
    Vector3d on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) {
        return on_unit_sphere;
    } else {
        return -on_unit_sphere;
    }
}

inline Vector3d reflect(const Vector3d& v, const Vector3d& n) { return v - 2 * dot(v, n) * n; }

inline Vector3d refract(const Vector3d& uv, const Vector3d& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    Vector3d r_out_perp = etai_over_etat * (uv + cos_theta * n);
    Vector3d r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

#endif  // VECTOR_3D_H