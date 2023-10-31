#ifndef COLOR_H
#define COLOR_H

#include <iostream>

#include "rtweekend.hpp"

using Color = Vector3d;

inline constexpr double linear_to_gamma(double linear_component) { return sqrt(linear_component); }

void write_color(std::ostream &out, const Color pixel_color, const double samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    auto scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    const Interval intensity = Interval(0, 0.999);

    out << static_cast<int>(256 * intensity.clamp(r)) << " "
        << static_cast<int>(256 * intensity.clamp(g)) << " "
        << static_cast<int>(256 * intensity.clamp(b)) << "\n";
}

#endif  // COLOR_H