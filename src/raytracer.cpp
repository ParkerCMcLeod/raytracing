#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>

#include "color.hpp"
#include "ray.hpp"
#include "vec3.hpp"

#define MAX_COLOR_VALUE 255
#define FILENAME "output/image.ppm"

color ray_color(const ray& r) {
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    // Open file stream to write the PPM file
    std::ofstream ppm_file(FILENAME);

    // Check if the file is opened successfully
    if (!ppm_file) {
        std::cerr << "Error: Could not open the file \"" << FILENAME << "\" for writing.\n";
        return 1;
    }

    // Image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    // Calculate the image height, and ensure that it's at least 1.
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // Camera
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double(image_width)/image_height);
    point3 camera_center = point3(0, 0, 0);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    // * INLINE https://raytracing.github.io/images/fig-1.04-pixel-grid.jpg
    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);
    
    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    point3 viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    point3 pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    ppm_file << "P3\n" << image_width << ' ' << image_height << "\n" << MAX_COLOR_VALUE << "\n";

    // Start processing the image
    for (int row = 0; row < image_height; row++) {
        for (int col = 0; col < image_width; col++) {
            auto pixel_center = pixel00_loc + (row * pixel_delta_u) + (col * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r);
            write_color(ppm_file, pixel_color);
        }

        // Log progress every row
        std::cout << "Processed row " << std::setw(3) << row + 1 << " / " << image_height << "\r" << std::flush;
    }

    ppm_file.close();

    // Measure the end time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    // Final completion message
    std::cout << "\nImage saved as " << FILENAME << "\n";
    std::cout << "Execution time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";

    return 0;
}
