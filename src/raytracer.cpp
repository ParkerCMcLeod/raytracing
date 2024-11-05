#include "rtweekend.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "sphere.hpp"

int main(void) {

    /* SCENE OBJECTS */

    // Create a list to store all objects (only spheres in our case) in the scene.
    hittable_list scene_objects = {};

    // Add a large ground sphere to represent the floor. Lambertian is the diffusion distribution method.
    auto ground_material = make_shared<lambertian>(color(0.2, 0.2, 0.2)); // Diffuse dark gray material
    scene_objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material)); // Large sphere as ground

    double large_sphere_radius = 1.0;
    double small_sphere_radius = 0.2;
    double possbile_interaction_radius = large_sphere_radius + small_sphere_radius;

    // Large glass-like sphere
    auto glass_material = make_shared<dielectric>(1.5);  
    scene_objects.add(make_shared<sphere>(point3(0, 1, 0), large_sphere_radius, glass_material));  

    // Large diffuse sphere
    auto diffuse_material = make_shared<lambertian>(color(0.4, 0.2, 0.1));  
    scene_objects.add(make_shared<sphere>(point3(-4, 1, 0), large_sphere_radius, diffuse_material));  

    // Large metal sphere
    auto metal_material = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);  
    scene_objects.add(make_shared<sphere>(point3(4, 1, 0), large_sphere_radius, metal_material));  

    // Generate small spheres randomly scattered across the ground.
    for (int x = -11; x < 11; x++) {
        for (int z = -11; z < 11; z++) {
            // Randomly choose a material for the current sphere.
            double random_material_choice = random_double();

            // Generate a random position for the sphere, slightly offset from grid positions.
            point3 sphere_center(x + 0.9 * random_double(), 0.2, z + 0.9 * random_double());

            // Ensure spheres don't overlap with the large spheres.
            if ((sphere_center - point3(4, 1, 0)).length() > possbile_interaction_radius && (sphere_center - point3(0, 1, 0)).length() > possbile_interaction_radius && (sphere_center - point3(-4, 1, 0)).length() > possbile_interaction_radius) {
                shared_ptr<material> sphere_material;

                // Choose a diffuse material (30% probability).
                if (random_material_choice < 0.3) {
                    auto albedo = color::random() * color::random(); // Random color for diffuse reflection
                    sphere_material = make_shared<lambertian>(albedo); // Diffuse material
                    scene_objects.add(make_shared<sphere>(sphere_center, small_sphere_radius, sphere_material)); // Add sphere

                // Choose a metal material (30% probability).
                } else if (random_material_choice < 0.6) {
                    auto albedo = color::random(0.5, 1); // Random metal color with some brightness
                    double fuzziness = random_double(0, 0.5); // Fuzziness affects the reflection blur
                    sphere_material = make_shared<metal>(albedo, fuzziness); // Metal material
                    scene_objects.add(make_shared<sphere>(sphere_center, small_sphere_radius, sphere_material)); // Add sphere

                // Choose a glass-like (dielectric) material (40% probability).
                } else {
                    sphere_material = make_shared<dielectric>(1.5); // Refractive index for glass-like material
                    scene_objects.add(make_shared<sphere>(sphere_center, small_sphere_radius, sphere_material)); // Add sphere
                }
            }
        }
    }

    /* CAMERA CONFIG */

    // Configure the camera.
    camera scene_camera;

    // Set basic camera parameters.
    scene_camera.aspect_ratio = 16.0 / 9.0; // Aspect ratio for widescreen rendering.
    scene_camera.image_width = 720; // Image width in pixels.
    scene_camera.samples_per_pixel = 10; // Samples per pixel for anti-aliasing.
    scene_camera.max_depth = 25; // Maximum recursion depth for reflections/refractions.

    // Set the camera's field of view and orientation.
    scene_camera.vertical_fov = 20; // Vertical field of view in degrees.
    scene_camera.camera_position = point3(13, 2, 3); // Camera position.
    scene_camera.focus_point = point3(0, 0, 0); // Target point the camera is looking at.
    scene_camera.up_direction = vec3(0, 1, 0); // Up direction for the camera (aligned with y-axis).

    // Configure depth of field by setting focus and aperture.
    scene_camera.lens_aperture = 0.2; // Aperture size affecting depth of field.
    scene_camera.focus_distance = 10.0; // Distance at which the camera is focused.

    /* RENDER SCENE */

    // Render the scene using the configured camera and objects.
    scene_camera.render(scene_objects);

    return 0;
}
