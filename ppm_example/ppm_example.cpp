#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <iomanip>

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    // Image dimensions
    int image_width = 256;
    int image_height = 256;
    int max_color_value = 255;
    char filename[256] = "ppm_example.ppm";

    // Open file stream to write the PPM file
    std::ofstream ppm_file(filename);

    // Check if the file is opened successfully
    if (!ppm_file) {
        std::cerr << "Error: Could not open the file \"" << filename << "\" for writing.\n";
        return 1;
    }

    ppm_file << "P3\n" << image_width << ' ' << image_height << "\n" << max_color_value << "\n";

    // Start processing the image
    for (int row = 0; row < image_height; row++) {
        for (int col = 0; col < image_width; col++) {
            int ir = ((col / static_cast<int>(std::sqrt(256))) % 2 == 
                      (row / static_cast<int>(std::sqrt(256))) % 2) ? 255 : 0;
            int ig = col;
            int ib = row;

            ppm_file << ir << ' ' << ig << ' ' << ib << '\n';
        }

        // Log progress every row
        std::cout << "Processed row " << std::setw(3) << row + 1 << " / " << image_height << "\r" << std::flush;
    }

    ppm_file.close();

    // Measure the end time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    // Final completion message
    std::cout << "\nImage saved as " << filename << "\n";
    std::cout << "Execution time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";

    return 0;
}
