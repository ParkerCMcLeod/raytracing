#include <iostream>
#include <fstream>
#include <cmath>

/*

PPM Format USED:

P3                            # means colors are ascii
[image_width] [image_height]  # cols / rows to parse
[max_color_value]             # max value representing full color expression
[r_value] [g_value] [b_value] # this triplet repeats [image_width * image_height] times to represent the final image

*/

int main() {
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

    for (int row = 0; row < image_height; row++) {
        for (int col = 0; col < image_width; col++) {
            int ir = ((col / static_cast<int>(std::sqrt(256))) % 2 == (row / static_cast<int>(std::sqrt(256))) % 2) ? 255 : 0;
            int ig = col;
            int ib = row;

            ppm_file << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    ppm_file.close();

    std::cout << "Image saved as ppm_example.ppm\n";

    return 0;
}