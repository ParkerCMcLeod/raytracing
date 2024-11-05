#ifndef INTERVAL_H
#define INTERVAL_H

#include "rtweekend.hpp"  // Import the required header, which likely includes necessary constants like `infinity`

// The `interval` class represents a range [min, max] on the real number line.
// This is useful in raytracing for defining bounds along which a ray might intersect objects.
class interval {
  public:
    double min = {}; // Lower bound of the interval
    double max = {}; // Upper bound of the interval

    // Default constructor: creates an "empty" interval with min > max
    // This convention is often used in raytracing to represent a region with no intersections.
    interval() : min(+infinity), max(-infinity) {}

    // Parameterized constructor: creates an interval with specified min and max bounds
    interval(double min, double max) : min(min), max(max) {}

    // Returns the size (or length) of the interval as max - min
    // This is helpful when calculating intersection lengths within an interval.
    double size() const {
        return max - min;
    }

    // Checks if a given value x lies within the interval, inclusive of min and max.
    // Useful for determining if a point lies within a given range.
    bool contains(double x) const {
        return min <= x && x <= max;
    }

    // Similar to `contains`, but excludes the bounds, meaning x must lie strictly between min and max.
    // Useful in cases where we need to ensure the value is within the interval but not on the boundary.
    bool surrounds(double x) const {
        return min < x && x < max;
    }

    // Clamps a value x to the interval: if x is less than min, returns min;
    // if x is greater than max, returns max; otherwise, returns x as-is.
    // This is often used to limit values to lie within specific bounds.
    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    // Two special static intervals are defined: `empty` and `universe`
    // `empty` is an interval with no space (min > max), used as a "null" intersection.
    // `universe` is an interval that spans the entire real line, useful for ray initialization.
    static const interval empty, universe;
};

// Define the special intervals outside of the class.
// `empty` interval: has min > max, representing an unbounded interval with no real points
const interval interval::empty    = interval(+infinity, -infinity);

// `universe` interval: spans all real values, with min = -∞ and max = +∞
const interval interval::universe = interval(-infinity, +infinity);

#endif
