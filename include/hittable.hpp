#ifndef HITTABLE_H
#define HITTABLE_H

// Forward declaration of the material class to avoid circular dependencies.
class material;

// `hit_record` stores information about a hit (or intersection) between a ray and a surface.
class hit_record {
  public:
    // Point of intersection (or hit point) in 3D space.
    point3 p = {};

    // The normal vector at the hit point, used to determine surface orientation and lighting effects.
    vec3 normal = {};

    // A pointer to the material of the object at the hit point, allowing the rendering system
    // to apply surface-specific properties such as reflection, refraction, or color.
    shared_ptr<material> mat = {};

    // The ray parameter `t` at which the intersection occurs. `t` represents the distance along the ray
    // from its origin, with larger values indicating farther points.
    double t = {};

    // A boolean indicating whether the surface normal is facing the ray origin (front face) or away from it.
    // This helps to distinguish between the two sides of a surface, enabling correct lighting calculations.
    bool front_face = {};

    // Sets the normal for the hit point so that it always points against the direction of the ray.
    // This ensures the normal is oriented outward from the surface, which is important for lighting calculations.
    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // `front_face` is set to true if the ray direction and outward normal are facing in opposite directions.
        // This indicates that the ray is hitting the "front" of the surface.
        front_face = dot(r.direction(), outward_normal) < 0;

        // If `front_face` is true, we set `normal` to the outward normal; otherwise, we invert it.
        // This guarantees that `normal` always points outward from the surface, regardless of ray direction.
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Abstract base class for any object that can be "hit" by a ray, e.g., a sphere or plane.
// The `hittable` class provides a polymorphic interface that specific shapes can inherit from.
class hittable {
  public:
    // Virtual destructor to allow safe deletion of derived objects via base class pointers.
    virtual ~hittable() = default;

    // The `hit` function is a pure virtual function that derived classes must implement.
    // It checks if the given ray intersects with the object within a specified range of distances (`ray_t`).
    // - `r` is the ray being cast.
    // - `ray_t` specifies the interval of `t` values to consider for intersections.
    // - `rec` is an output parameter used to store hit information if an intersection occurs.
    // The function returns true if the ray hits the object; otherwise, it returns false.
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif
