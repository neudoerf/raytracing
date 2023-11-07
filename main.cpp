#include <future>
#include <iostream>
#include <thread>

#include "bvh.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "constant_medium.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "quad.hpp"
#include "rtweekend.hpp"
#include "sphere.hpp"
#include "texture.hpp"

std::vector<std::vector<Color>> render(const Hittable& world, Camera cam) {
    return cam.render(world);
}

void random_spheres(HittableList& world, Camera& cam) {
    auto checker = make_shared<CheckerTexture>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
    world.add(make_shared<Sphere>(Point3d(0, -1000, 0), 1000, make_shared<Lambertian>(checker)));

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

    world = HittableList(std::make_shared<BvhNode>(world));

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = Color(0.7, 0.8, 1);

    cam.vfov = 20;
    cam.look_from = Point3d(13, 2, 3);
    cam.look_at = Point3d(0, 0, 0);
    cam.v_up = Point3d(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;
}

void two_spheres(HittableList& world, Camera& cam) {
    auto checker = make_shared<CheckerTexture>(0.8, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));

    world.add(make_shared<Sphere>(Point3d(0, -10, 0), 10, make_shared<Lambertian>(checker)));
    world.add(make_shared<Sphere>(Point3d(0, 10, 0), 10, make_shared<Lambertian>(checker)));

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = Color(0.7, 0.8, 1);

    cam.vfov = 20;
    cam.look_from = Point3d(13, 2, 3);
    cam.look_at = Point3d(0, 0, 0);
    cam.v_up = Point3d(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;
}

void earth(HittableList& world, Camera& cam) {
    auto earth_texture = make_shared<ImageTexture>("earthmap.jpg");
    auto earth_surface = make_shared<Lambertian>(earth_texture);
    auto globe = make_shared<Sphere>(Point3d(0, 0, 0), 2, earth_surface);

    world.add(globe);

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = Color(0.7, 0.8, 1);

    cam.vfov = 20;
    cam.look_from = Point3d(13, 2, 3);
    cam.look_at = Point3d(0, 0, 0);
    cam.v_up = Point3d(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;
}

void two_perlin_spheres(HittableList& world, Camera& cam) {
    auto pertext = make_shared<NoiseTexture>(4);
    world.add(make_shared<Sphere>(Point3d(0, -1000, 0), 1000, make_shared<Lambertian>(pertext)));
    world.add(make_shared<Sphere>(Point3d(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = Color(0.7, 0.8, 1);

    cam.vfov = 20;
    cam.look_from = Point3d(13, 2, 3);
    cam.look_at = Point3d(0, 0, 0);
    cam.v_up = Point3d(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;
}

void quads(HittableList& world, Camera& cam) {
    auto left_red = make_shared<Lambertian>(Color(1, 0.2, 0.2));
    auto back_green = make_shared<Lambertian>(Color(0.2, 1, 0.2));
    auto right_blue = make_shared<Lambertian>(Color(0.2, 0.2, 1));
    auto upper_orange = make_shared<Lambertian>(Color(1, 0.5, 0));
    auto lower_teal = make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

    world.add(
        make_shared<Quad>(Point3d(-3, -2, 5), Vector3d(0, 0, -4), Vector3d(0, 4, 0), left_red));
    world.add(
        make_shared<Quad>(Point3d(-2, -2, 0), Vector3d(4, 0, 0), Vector3d(0, 4, 0), back_green));
    world.add(
        make_shared<Quad>(Point3d(3, -2, 1), Vector3d(0, 0, 4), Vector3d(0, 4, 0), right_blue));
    world.add(
        make_shared<Quad>(Point3d(-2, 3, 1), Vector3d(4, 0, 0), Vector3d(0, 0, 4), upper_orange));
    world.add(
        make_shared<Quad>(Point3d(-2, -3, 5), Vector3d(4, 0, 0), Vector3d(0, 0, -4), lower_teal));

    cam.aspect_ratio = 1.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = Color(0.7, 0.8, 1);

    cam.vfov = 80;
    cam.look_from = Point3d(0, 0, 9);
    cam.look_at = Point3d(0, 0, 0);
    cam.v_up = Vector3d(0, 1, 0);

    cam.defocus_angle = 0;
}

void simple_light(HittableList& world, Camera& cam) {
    auto pertext = make_shared<NoiseTexture>(4);
    world.add(make_shared<Sphere>(Point3d(0, -1000, 0), 1000, make_shared<Lambertian>(pertext)));
    world.add(make_shared<Sphere>(Point3d(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

    auto difflight = make_shared<DiffuseLight>(Color(4, 4, 4));
    world.add(
        make_shared<Quad>(Point3d(3, 1, -2), Vector3d(2, 0, 0), Vector3d(0, 2, 0), difflight));

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = Color(0, 0, 0);

    cam.vfov = 20;
    cam.look_from = Point3d(26, 3, 6);
    cam.look_at = Point3d(0, 2, 0);
    cam.v_up = Vector3d(0, 1, 0);

    cam.defocus_angle = 0;
}

void cornell_box(HittableList& world, Camera& cam) {
    auto red = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

    world.add(
        make_shared<Quad>(Point3d(555, 0, 0), Vector3d(0, 555, 0), Vector3d(0, 0, 555), green));
    world.add(make_shared<Quad>(Point3d(0, 0, 0), Vector3d(0, 555, 0), Vector3d(0, 0, 555), red));
    world.add(make_shared<Quad>(Point3d(343, 554, 332), Vector3d(-130, 0, 0), Vector3d(0, 0, -105),
                                light));
    world.add(make_shared<Quad>(Point3d(0, 0, 0), Vector3d(555, 0, 0), Vector3d(0, 0, 555), white));
    world.add(make_shared<Quad>(Point3d(555, 555, 555), Vector3d(-555, 0, 0), Vector3d(0, 0, -555),
                                white));
    world.add(
        make_shared<Quad>(Point3d(0, 0, 555), Vector3d(555, 0, 0), Vector3d(0, 555, 0), white));

    shared_ptr<Hittable> box1 = box(Point3d(0, 0, 0), Point3d(165, 330, 165), white);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vector3d(265, 0, 295));
    world.add(box1);

    shared_ptr<Hittable> box2 = box(Point3d(0, 0, 0), Point3d(165, 165, 165), white);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vector3d(130, 0, 65));
    world.add(box2);

    cam.aspect_ratio = 1.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;
    cam.background = Color(0, 0, 0);

    cam.vfov = 40;
    cam.look_from = Point3d(278, 278, -800);
    cam.look_at = Point3d(278, 278, 0);
    cam.v_up = Vector3d(0, 1, 0);

    cam.defocus_angle = 0;
}

void cornell_smoke(HittableList& world, Camera& cam) {
    auto red = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Color(7, 7, 7));

    world.add(
        make_shared<Quad>(Point3d(555, 0, 0), Vector3d(0, 555, 0), Vector3d(0, 0, 555), green));
    world.add(make_shared<Quad>(Point3d(0, 0, 0), Vector3d(0, 555, 0), Vector3d(0, 0, 555), red));
    world.add(
        make_shared<Quad>(Point3d(113, 554, 127), Vector3d(330, 0, 0), Vector3d(0, 0, 305), light));
    world.add(make_shared<Quad>(Point3d(0, 0, 0), Vector3d(555, 0, 0), Vector3d(0, 0, 555), white));
    world.add(make_shared<Quad>(Point3d(555, 555, 555), Vector3d(-555, 0, 0), Vector3d(0, 0, -555),
                                white));
    world.add(
        make_shared<Quad>(Point3d(0, 0, 555), Vector3d(555, 0, 0), Vector3d(0, 555, 0), white));

    shared_ptr<Hittable> box1 = box(Point3d(0, 0, 0), Point3d(165, 330, 165), white);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vector3d(265, 0, 295));

    shared_ptr<Hittable> box2 = box(Point3d(0, 0, 0), Point3d(165, 165, 165), white);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vector3d(130, 0, 65));

    world.add(make_shared<ConstantMedium>(box1, 0.01, Color(0, 0, 0)));
    world.add(make_shared<ConstantMedium>(box2, 0.01, Color(1, 1, 1)));

    cam.aspect_ratio = 1.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;
    cam.background = Color(0, 0, 0);

    cam.vfov = 40;
    cam.look_from = Point3d(278, 278, -800);
    cam.look_at = Point3d(278, 278, 0);
    cam.v_up = Vector3d(0, 1, 0);

    cam.defocus_angle = 0;
}

void final_scene(int image_width, int samples_per_pixel, int max_depth, HittableList& world,
                 Camera& cam) {
    HittableList boxes1;
    auto ground = make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(box(Point3d(x0, y0, z0), Point3d(x1, y1, z1), ground));
        }
    }

    world.add(make_shared<BvhNode>(boxes1));

    auto light = make_shared<DiffuseLight>(Color(7, 7, 7));
    world.add(
        make_shared<Quad>(Point3d(123, 554, 147), Vector3d(300, 0, 0), Vector3d(0, 0, 265), light));

    auto center1 = Point3d(400, 400, 200);
    auto center2 = center1 + Vector3d(30, 0, 0);
    auto sphere_material = make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
    world.add(make_shared<Sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<Sphere>(Point3d(260, 150, 45), 50, make_shared<Dielectric>(1.5)));
    world.add(make_shared<Sphere>(Point3d(0, 150, 145), 50,
                                  make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)));

    auto boundary = make_shared<Sphere>(Point3d(360, 150, 145), 70, make_shared<Dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
    boundary = make_shared<Sphere>(Point3d(0, 0, 0), 5000, make_shared<Dielectric>(1.5));
    world.add(make_shared<ConstantMedium>(boundary, 0.0001, Color(1, 1, 1)));

    auto emat = make_shared<Lambertian>(make_shared<ImageTexture>("earthmap.jpg"));
    world.add(make_shared<Sphere>(Point3d(400, 200, 400), 100, emat));
    auto pertext = make_shared<NoiseTexture>(0.1);
    world.add(make_shared<Sphere>(Point3d(220, 280, 300), 80, make_shared<Lambertian>(pertext)));

    HittableList boxes2;
    auto white = make_shared<Lambertian>(Color(0.73, 0.73, 0.73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<Sphere>(Point3d::random(0, 165), 10, white));
    }

    world.add(make_shared<Translate>(make_shared<RotateY>(make_shared<BvhNode>(boxes2), 15),
                                     Vector3d(-100, 270, 395)));

    cam.aspect_ratio = 1.0;
    cam.image_width = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = max_depth;
    cam.background = Color(0, 0, 0);

    cam.vfov = 40;
    cam.look_from = Point3d(478, 278, -600);
    cam.look_at = Point3d(278, 278, 0);
    cam.v_up = Vector3d(0, 1, 0);

    cam.defocus_angle = 0;
}

int main(int, char**) {
    HittableList world;
    Camera cam;
    int SCENE = 10;

    switch (SCENE) {
        case 1:
            random_spheres(world, cam);
            break;
        case 2:
            two_spheres(world, cam);
            break;
        case 3:
            earth(world, cam);
            break;
        case 4:
            two_perlin_spheres(world, cam);
            break;
        case 5:
            quads(world, cam);
            break;
        case 6:
            simple_light(world, cam);
            break;
        case 7:
            cornell_box(world, cam);
            break;
        case 8:
            cornell_smoke(world, cam);
            break;
        case 9:
            final_scene(800, 10000, 40, world, cam);
            break;
        default:
            final_scene(400, 250, 4, world, cam);
    }

    int NUM_THREADS = 4;
    int SAMPLES_PER_PIXEL = cam.samples_per_pixel / NUM_THREADS;
    cam.samples_per_pixel = SAMPLES_PER_PIXEL;
    std::vector<std::future<std::vector<std::vector<Color>>>> futures(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; ++i) {
        futures[i] = std::async(std::launch::async, render, world, cam);
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
