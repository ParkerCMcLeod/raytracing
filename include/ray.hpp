#ifndef RAY_H
#define RAY_H

#include "vec3.hpp"

// The ray class represents a mathematical ray in 3D space, which is defined by an
// origin point and a direction vector. This class will be central in ray tracing,
// where rays are used to sample the environment (e.g., check for intersections with objects).
class ray {
  public:
    // Default constructor: Initializes a ray with no specific origin or direction.
    // This might be used if we need to declare a ray without immediately defining it.
    ray() {}

    // Parameterized constructor: Initializes a ray with a specified origin and direction.
    // Parameters:
    // - origin: A point in 3D space where the ray starts.
    // - direction: A vector that represents the direction of the ray.
    ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

    // Returns the origin of the ray.
    // The origin is the starting point of the ray in 3D space.
    const point3& origin() const  { return orig; }

    // Returns the direction of the ray.
    // The direction vector represents the direction the ray is traveling.
    const vec3& direction() const { return dir; }

    // Calculates a point along the ray at a given distance `t`.
    // The formula is derived from the parametric equation of a line in 3D space:
    // Point_at_t = Origin + t * Direction
    // where:
    // - `t` is a parameter that scales the direction vector.
    // - `orig + t*dir` gives us a new point along the ray.
    // Parameters:
    // - t: A scalar value that scales the direction vector to determine how far along
    //      the ray the point is from the origin.
    point3 at(double t) const {
        return orig + t*dir;
    }

  private:
    // The origin of the ray, a point in 3D space. This is where the ray starts.
    point3 orig = {};

    // The direction of the ray, a vector in 3D space. This vector is normalized (typically
    // has a length of 1) and indicates the direction in which the ray is traveling.
    vec3 dir = {};
};

#endif
