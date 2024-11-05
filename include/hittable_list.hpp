#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.hpp"
#include <vector>

// hittable_list class serves as a container for multiple hittable objects.
// It inherits from the hittable class, making it a polymorphic object
// that can represent any collection of hittable objects (e.g., spheres, planes, etc.).

class hittable_list : public hittable {
public:
    // A vector to store pointers to hittable objects.
    // shared_ptr is used to ensure proper memory management.
    std::vector<shared_ptr<hittable>> objects;

    // Default constructor for an empty hittable list.
    hittable_list() {}

    // Constructor that adds an initial object to the list.
    hittable_list(shared_ptr<hittable> object) { add(object); }

    // Method to clear all objects from the list.
    // This is useful for resetting or reusing the list without creating a new instance.
    void clear() { objects.clear(); }

    // Method to add a hittable object to the list.
    // Accepts a shared_ptr to an object derived from the hittable class.
    void add(shared_ptr<hittable> object) {
        objects.push_back(object);
    }

    // Override the hit function inherited from the hittable class.
    // This function checks if any object in the list is hit by a given ray 'r'.
    // Parameters:
    // - const ray& r: The ray being cast.
    // - interval ray_t: The range of t values (parameterized distance along the ray) to check.
    // - hit_record& rec: Struct to store details of the closest hit point.
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec = {}; // Temporary record to store each object's hit data.
        bool hit_anything = false; // Boolean to track if any object was hit.
        auto closest_so_far = ray_t.max; // Tracks the closest hit distance.

        // Loop through all objects in the list.
        for (const auto& object : objects) {
            // Check if the current object is hit by the ray within the range (ray_t.min, closest_so_far).
            // This range narrowing ensures we only consider the closest hit.
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;             // Mark that at least one hit was detected.
                closest_so_far = temp_rec.t;     // Update the closest distance to the new hit.
                rec = temp_rec;                  // Store this hit record as it is the closest so far.
            }
        }

        // Return true if any object was hit, false otherwise.
        return hit_anything;
    }
};

#endif
