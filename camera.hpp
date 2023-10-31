#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

#include "color.hpp"
#include "hittable.hpp"
#include "material.hpp"
#include "rtweekend.hpp"

class Camera {
public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10;

    double vfov = 90;
    Point3d look_from = Point3d(0, 0, -1);
    Point3d look_at = Point3d(0, 0, 0);
    Vector3d v_up = Vector3d(0, 1, 0);

    double defocus_angle = 0;
    double focus_dist = 10;

    std::vector<std::vector<Color>> render(const Hittable& world) {
        initialize();

        std::vector<std::vector<Color>> output(image_height,
                                               std::vector<Color>(image_width, Color()));

        // std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                Color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    Ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }

                output[j][i] = pixel_color;
            }
        }

        std::clog << "\rDone.                 \n";

        return output;
    }

private:
    int image_height;
    Point3d center;
    Point3d pixel00_loc;
    Vector3d pixel_delta_u;
    Vector3d pixel_delta_v;
    Vector3d u, v, w;
    Vector3d defocus_disk_u;
    Vector3d defocus_disk_v;

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = look_from;

        // determine viewport dimensions
        auto theta = deg_to_rad(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        // calculate the u, v, w unit basis vectors for the camera coordinate frame
        w = unit_vector(look_from - look_at);
        u = unit_vector(cross(v_up, w));
        v = cross(w, u);

        // calculate the vectors across teh horizontal and down the vertical viewport edges
        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * -v;

        // calculate the horizontal and vertical delta vectors from pixel to pixel
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // calculate the location of the upper left pixel
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u * pixel_delta_v);

        // calculate the camera defocus disk basis vectors
        auto defocus_radius = focus_dist * tan(deg_to_rad(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    Color ray_color(const Ray& r, int depth, const Hittable& world) const {
        HitRecord rec;

        if (depth <= 0) {
            return Color(0, 0, 0);
        }

        if (world.hit(r, Interval(0.001, infinity), rec)) {
            Ray scattered;
            Color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                return attenuation * ray_color(scattered, depth - 1, world);
            }
            return Color(0, 0, 0);
        }

        Vector3d unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * Color(1, 1, 1) + a * Color(0.5, 0.7, 1);
    }

    Ray get_ray(int i, int j) const {
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }

    Vector3d pixel_sample_square() const {
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    Point3d defocus_disk_sample() const {
        auto p = random_in_unit_disk();
        return center + (p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
    }
};

#endif  // CAMERA_H