#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.hpp"
#include "material.hpp"

class camera {
public:
    // Camera parameters and image settings
    double aspect_ratio = 1.0; // Image width divided by height
    int image_width = 100; // Width of the image in pixels
    int samples_per_pixel = 10; // Number of random samples per pixel for anti-aliasing
    int max_depth = 10; // Maximum recursion depth for ray bouncing

    double vertical_fov = 90; // Vertical field of view (in degrees)
    point3 camera_position = point3(0, 0, 0); // Camera position in 3D space
    point3 focus_point = point3(0, 0, -1); // The point the camera looks at
    vec3 up_direction = vec3(0, 1, 0); // "Up" direction relative to the camera

    double lens_aperture = 0; // Aperture controlling depth of field (defocus blur)
    double focus_distance = 10; // Distance to the focal plane (sharp focus)

    // Renders the scene using the provided world of hittable objects
    void render(const hittable& scene) {
        initialize();
        
        // Start measuring time
        auto start_time = std::chrono::high_resolution_clock::now();
        auto last_update_time = start_time; // Track the last time we updated the log

        std::ofstream output_file("output/image.ppm"); // Open file for output

        if (!output_file) { // Check if the file opened successfully
            std::cerr << "Error: Could not open output/image.ppm for writing.\n";
            return;
        }

        // Write PPM file header
        output_file << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        // Variables for time estimation
        int rows_remaining = image_height; // Track rows left to process

        // Loop over each row (scanline) from top to bottom
        for (int row = 0; row < image_height; ++row) {
            // Process the current row
            for (int col = 0; col < image_width; ++col) {
                color accumulated_color(0, 0, 0); // Initialize color for this pixel

                // Anti-aliasing: Take multiple samples per pixel
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    ray pixel_ray = generate_ray(col, row); // Generate a ray for this pixel
                    accumulated_color += trace_ray(pixel_ray, max_depth, scene); // Accumulate color
                }

                // Write the averaged color to the output file
                write_color(output_file, scale_color * accumulated_color);
            }

            // Check if one second has passed since the last update
            auto current_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_since_last_update = current_time - last_update_time;

            if (elapsed_since_last_update.count() >= 1.0) { // Update only once per second at most
                // Calculate time per row and estimate remaining time
                std::chrono::duration<double, std::milli> elapsed = current_time - start_time;
                double avg_time_per_row = elapsed.count() / (row + 1); // Average time per row
                double remaining_time_ms = avg_time_per_row * (rows_remaining - 1); // Remaining time in ms

                // Format the estimated time remaining
                int remaining_seconds = static_cast<int>(remaining_time_ms / 1000) % 60;
                int remaining_minutes = static_cast<int>(remaining_time_ms / (1000 * 60));

                // Log progress with estimated time remaining
                std::clog << "\rScanlines remaining: " << (image_height - row - 1) << " | Estimated time left: " << remaining_minutes << "m " << remaining_seconds << "s" << std::flush;

                // Update the timestamp for the next log update
                last_update_time = current_time;
            }

            rows_remaining--; // Decrement remaining rows
        }

        output_file.close(); // Close the output file
        std::clog << "\rDone.                                                                                   \n"; // Log completion

        // Measure and print the total render time
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> render_time = end_time - start_time;

        std::cout << "\nImage saved as output/image.ppm\n";
        std::cout << "Render time: " << std::fixed << std::setprecision(2) << render_time.count() << " ms\n";
    }

private:
    // Private member variables
    int image_height = {}; // Height of the image (derived from width and aspect ratio)
    double scale_color = {}; // Scaling factor for averaging pixel samples
    point3 upper_left_pixel = {}; // World space location of the upper-left corner of the image
    vec3 horizontal_pixel_step = {}; // Vector step to move one pixel to the right
    vec3 vertical_pixel_step = {}; // Vector step to move one pixel down
    vec3 camera_basis_u = {}; // Camera coordinate system basis vectors
    vec3 camera_basis_v = {}; // Camera coordinate system basis vectors
    vec3 camera_basis_w = {}; // Camera coordinate system basis vectors
    vec3 aperture_disk_u = {}; // Aperture disk basis vectors for lens blur
    vec3 aperture_disk_v = {}; // Aperture disk basis vectors for lens blur

    // Initializes the camera properties and viewport
    void initialize() {
        // Calculate image height based on the aspect ratio
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = std::max(1, image_height); // Ensure height is at least 1 pixel

        scale_color = 1.0 / samples_per_pixel; // Scale factor for color averaging

        // Calculate field of view in radians and the viewport dimensions
        double theta = degrees_to_radians(vertical_fov);
        double half_height = std::tan(theta / 2);
        double viewport_height = 2.0 * half_height * focus_distance;
        double viewport_width = viewport_height * aspect_ratio;

        // Calculate the camera coordinate system (orthonormal basis vectors)
        camera_basis_w = unit_vector(camera_position - focus_point); // Camera "forward" direction
        camera_basis_u = unit_vector(cross(up_direction, camera_basis_w)); // "Right" direction
        camera_basis_v = cross(camera_basis_w, camera_basis_u); // "Up" direction

        // Calculate the horizontal and vertical vectors spanning the viewport
        vec3 horizontal_span = viewport_width * camera_basis_u;
        vec3 vertical_span = -viewport_height * camera_basis_v;

        // Calculate the position of the upper-left pixel in world space
        upper_left_pixel = camera_position - (focus_distance * camera_basis_w) - (horizontal_span / 2) - (vertical_span / 2);

        // Calculate per-pixel steps in world space
        horizontal_pixel_step = horizontal_span / image_width;
        vertical_pixel_step = vertical_span / image_height;

        // Compute aperture disk vectors for simulating depth of field
        double aperture_radius = focus_distance * std::tan(degrees_to_radians(lens_aperture / 2));
        aperture_disk_u = aperture_radius * camera_basis_u;
        aperture_disk_v = aperture_radius * camera_basis_v;
    }

    // Generates a ray for a specific pixel (col, row) with optional lens blur
    ray generate_ray(int col, int row) const {
        // Random offset for anti-aliasing
        vec3 pixel_offset = sample_unit_square();

        // Calculate the target location in the scene for the current pixel
        point3 target_pixel = upper_left_pixel + (col + pixel_offset.x()) * horizontal_pixel_step + (row + pixel_offset.y()) * vertical_pixel_step;

        // Calculate the ray's origin (accounting for lens blur)
        point3 ray_origin = (lens_aperture > 0) ? sample_aperture_disk() : camera_position;

        // Ray direction from origin to target pixel
        vec3 ray_direction = target_pixel - ray_origin;

        return ray(ray_origin, ray_direction); // Return the generated ray
    }

    // Samples a random point within a unit square for anti-aliasing
    vec3 sample_unit_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    // Samples a random point within the aperture disk for depth of field simulation
    point3 sample_aperture_disk() const {
        vec3 random_point = random_in_unit_disk();
        return camera_position + random_point.x() * aperture_disk_u + random_point.y() * aperture_disk_v;
    }

    // Traces a ray through the scene, returning the color based on intersections
    color trace_ray(const ray& r, int depth, const hittable& scene) const {
        if (depth <= 0) // Stop recursion if max depth is reached
            return color(0, 0, 0); // Return black (no more light)

        hit_record record = {}; // Record of the intersection

        // Check for intersection with the scene
        if (scene.hit(r, interval(0.001, infinity), record)) {
            ray scattered; // Scattered ray after intersection
            color attenuation; // How much the material attenuates light
            if (record.mat->scatter(r, record, attenuation, scattered))
                return attenuation * trace_ray(scattered, depth - 1, scene); // Recursively trace
            return color(0, 0, 0); // Return black if no scattering
        }

        // If no hit, compute background color (gradient from white to blue)
        vec3 unit_direction = unit_vector(r.direction());
        double t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
    }
};

#endif