#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.hpp"
#include "rtw_stb_image.hpp"
#include "rtweekend.hpp"

class Texture {
public:
    virtual ~Texture() = default;

    virtual Color value(double u, double v, const Point3d& p) const = 0;
};

class SolidColor : public Texture {
public:
    SolidColor(Color c) : color_value(c) {}

    SolidColor(double red, double green, double blue) : SolidColor(Color(red, green, blue)) {}

    Color value(double u, double v, const Point3d& p) const override { return color_value; }

private:
    Color color_value;
};

class CheckerTexture : public Texture {
public:
    CheckerTexture(double _scale, std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd)
        : inv_scale(1.0 / _scale), even(_even), odd(_odd) {}

    CheckerTexture(double _scale, Color c1, Color c2)
        : inv_scale(1.0 / _scale),
          even(std::make_shared<SolidColor>(c1)),
          odd(std::make_shared<SolidColor>(c2)) {}

    Color value(double u, double v, const Point3d& p) const override {
        auto x_int = static_cast<int>(std::floor(inv_scale * p.x()));
        auto y_int = static_cast<int>(std::floor(inv_scale * p.y()));
        auto z_int = static_cast<int>(std::floor(inv_scale * p.z()));

        bool is_even = (x_int + y_int + z_int) % 2 == 0;

        return is_even ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
};

class ImageTexture : public Texture {
public:
    ImageTexture(const char* filename) : image(filename) {}

    Color value(double u, double v, const Point3d& p) const override {
        if (image.height() <= 0) {
            return Color(0, 1, 1);
        }

        u = Interval(0, 1).clamp(u);
        v = 1.0 - Interval(0, 1).clamp(v);

        auto i = static_cast<int>(u * image.width());
        auto j = static_cast<int>(v * image.height());
        auto pixel = image.pixel_data(i, j);

        auto color_scale = 1.0 / 255.0;

        return Color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    RtwImage image;
};

#endif  // TEXTURE_H