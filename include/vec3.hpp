#ifndef VEC3_H
#define VEC3_H

#include <cmath> // Required for sqrt and fabs functions

// vec3 class represents a 3D vector with basic vector operations, commonly used in graphics and physics
class vec3 {
  public:
    double e[3] = {}; // Stores the three components of the vector (x, y, z)

    // Default constructor: initializes the vector to (0,0,0)
    vec3() : e{0,0,0} {}

    // Parameterized constructor: initializes the vector to (e0, e1, e2)
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    // Accessor functions to retrieve each component
    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    // Unary negation operator to invert each component of the vector
    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

    // Index operators to get and set components of the vector
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    // Vector addition and assignment
    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this; // Return the current vector with updated values
    }

    // Scalar multiplication and assignment (scales each component by t)
    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    // Scalar division and assignment (divides each component by t)
    vec3& operator/=(double t) {
        return *this *= 1/t; // Dividing by t is equivalent to multiplying by 1/t
    }

    // Returns the length (magnitude) of the vector
    double length() const {
        return std::sqrt(length_squared());
    }

    // Returns the square of the length, which is faster to compute and useful for comparison
    double length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    // Checks if the vector is close to zero in all dimensions
    bool near_zero() const {
        // Define a small threshold value (1e-8) for comparison
        auto s = 1e-8;
        return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
    }

    // Generates a random vector with each component between 0 and 1
    static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    // Generates a random vector with each component between min and max
    static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

// Define point3 as an alias for vec3 to represent points in 3D space (for semantic clarity)
using point3 = vec3;

// Vector Utility Functions

// Vector addition
inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

// Vector subtraction
inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

// Component-wise vector multiplication (not dot product)
inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

// Scalar multiplication (scales the vector by t)
inline vec3 operator*(double t, const vec3& v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

// Scalar multiplication in reversed order (for symmetry)
inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

// Scalar division (scales the vector by 1/t)
inline vec3 operator/(const vec3& v, double t) {
    return (1/t) * v;
}

// Dot product: returns a scalar representing the cosine of the angle between u and v
inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

// Cross product: returns a vector perpendicular to both u and v
inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// Returns the unit vector in the same direction as v
inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

// Generates a random point inside a unit disk (for certain types of ray origins)
inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0); // Generate a 2D point
        if (p.length_squared() < 1) // Check if point is within unit disk
            return p;
    }
}

// Generates a random unit vector (useful for sampling directions on a sphere)
inline vec3 random_unit_vector() {
    while (true) {
        auto p = vec3::random(-1, 1);
        auto lensq = p.length_squared();
        if (1e-160 < lensq && lensq <= 1.0) // Ensure non-zero length and unit length
            return p / sqrt(lensq);
    }
}

// Returns a random vector in the same hemisphere as the given normal vector
inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector(); // Random direction on unit sphere
    if (dot(on_unit_sphere, normal) > 0.0) // Check if in the same hemisphere
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

// Reflects vector v about normal n, used for simulating mirror-like reflections
inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

// Refracts vector uv through a surface with normal n given an index of refraction ratio (Snell's law)
inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = std::fmin(dot(-uv, n), 1.0); // Angle between vector and normal
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n); // Perpendicular component
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n; // Parallel component
    return r_out_perp + r_out_parallel; // Total refracted vector
}

#endif
