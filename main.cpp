#include <future>
#include <iostream>
#include <thread>

#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "rtweekend.hpp"
#include "sphere.hpp"

std::vector<std::vector<Color>> render(const Hittable& world, const int samples_per_pixel) {
    Camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.look_from = Point3d(13, 2, 3);
    cam.look_at = Point3d(0, 0, 0);
    cam.v_up = Point3d(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    return cam.render(world);
}

int main(int, char**) {
    // world
    HittableList world;

    auto mat_ground = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    world.add(make_shared<Sphere>(Point3d(0, -1000, 0), 1000, mat_ground));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            Point3d center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - Point3d(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Material> mat_sphere;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Color::random() * Color::random();
                    mat_sphere = make_shared<Lambertian>(albedo);
                    auto center2 = center + Vector3d(0, random_double(0, 0.5), 0);
                    world.add(make_shared<Sphere>(center, center2, 0.2, mat_sphere));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    mat_sphere = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, 0.2, mat_sphere));
                } else {
                    // glass
                    mat_sphere = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, 0.2, mat_sphere));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(Point3d(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(Point3d(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(Point3d(4, 1, 0), 1.0, material3));

    int NUM_THREADS = 4;
    int SAMPLES_PER_PIXEL = 25;
    std::vector<std::future<std::vector<std::vector<Color>>>> futures(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; ++i) {
        futures[i] = std::async(std::launch::async, render, world, SAMPLES_PER_PIXEL);
    }

    std::vector<std::vector<std::vector<Color>>> results(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; ++i) {
        results[i] = futures[i].get();
    }

    auto image_width = results[0][0].size();
    auto image_height = results[0].size();

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int i = 0; i < image_height; ++i) {
        for (int j = 0; j < image_width; ++j) {
            Color pixel_color(0, 0, 0);
            for (int k = 0; k < NUM_THREADS; ++k) {
                pixel_color += results[k][i][j];
            }
            write_color(std::cout, pixel_color, NUM_THREADS * SAMPLES_PER_PIXEL);
        }
    }
}
