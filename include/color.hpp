#ifndef COLOR_H
#define COLOR_H

// Include the necessary headers for interval clamping and vector operations.
#include "interval.hpp"
#include "vec3.hpp"

// Define 'color' as an alias for 'vec3', representing RGB color values.
using color = vec3;

// Function to convert a linear color component to gamma-corrected value.
// This adjusts the brightness for display, accounting for how monitors
// interpret brightness, making the color appear more natural.
inline double linear_to_gamma(double linear_component)
{
    // If the component is positive, apply the gamma correction.
    // Gamma 2 correction is achieved by taking the square root.
    if (linear_component > 0)
        return std::sqrt(linear_component);  // Apply gamma correction for gamma = 2.

    // If the component is zero or negative, return zero as a safe fallback.
    return 0;
}

// Function to output a color value to a stream, converting it to 8-bit values.
void write_color(std::ostream& out, const color& pixel_color) {
    // Extract RGB components from the color object.
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Apply a linear-to-gamma transformation with gamma = 2.
    // This makes colors appear less washed-out when displayed.
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Convert the normalized [0,1] range values to the [0,255] byte range.
    // We use an interval clamp to ensure values stay within [0,1] to avoid overflow.
    static const interval intensity(0.000, 0.999); // Clamp to slightly under 1 to avoid rounding issues.
    
    // Convert each component to a byte value by scaling and clamping.
    int rbyte = int(256 * intensity.clamp(r)); // Red component in the range [0,255]
    int gbyte = int(256 * intensity.clamp(g)); // Green component in the range [0,255]
    int bbyte = int(256 * intensity.clamp(b)); // Blue component in the range [0,255]

    // Write out the RGB color components as integers separated by spaces.
    // Each color channel value is written as an 8-bit integer, creating
    // the final pixel color in a format suitable for PPM or similar image formats.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif 
