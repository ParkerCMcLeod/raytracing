#include <iostream>
#include <fstream>
#include <cmath>

int main() {
    // Image dimensions
    int image_width = 256;
    int image_height = 256;

    // Open file stream to write the PPM file
    std::ofstream ppm_file("ppm_example.ppm");

    // Check if the file is opened successfully
    if (!ppm_file) {
        std::cerr << "Error: Could not open the file for writing.\n";
        return 1;
    }

    ppm_file << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            int ir = ((i / static_cast<int>(std::sqrt(256))) % 2 == (j / static_cast<int>(std::sqrt(256))) % 2) ? 255 : 0;
            int ig = i;
            int ib = j;

            ppm_file << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    ppm_file.close();

    std::cout << "Image saved as ppm_example.ppm\n";

    return 0;
}