#ifndef SPHERE_H
#define SPHERE_H

// The sphere class represents a sphere in 3D space and inherits from the 'hittable' base class.
// Each sphere object has a center, a radius, and a material.
// The 'hit' method checks if a given ray intersects the sphere and records the hit details if it does.

class sphere : public hittable {
  public:
    // Constructor to initialize the sphere's center, radius, and material.
    // Radius is clamped to zero or positive to prevent invalid shapes.
    sphere(const point3& center, double radius, shared_ptr<material> mat)
      : center(center), radius(std::fmax(0,radius)), mat(mat) {}

    // Method to determine if a ray hits the sphere within a given interval.
    // Takes a ray (r), an interval (ray_t), and a hit record (rec) to store hit details.
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // Compute the vector from the ray origin to the center of the sphere.
        vec3 oc = center - r.origin();

        // 'a' represents the squared length of the ray direction vector.
        // This value is used to normalize the direction component in the discriminant.
        auto a = r.direction().length_squared();

        // 'h' is the projection of 'oc' onto the ray direction.
        // It represents half the length of the vector along the ray's direction
        // to the closest approach to the sphere center.
        auto h = dot(r.direction(), oc);

        // 'c' is the squared distance from the ray origin to the sphere center,
        // adjusted by the sphere's radius. This is needed to calculate the discriminant.
        auto c = oc.length_squared() - radius*radius;

        // Compute the discriminant to determine if the ray intersects the sphere.
        // If the discriminant is negative, there is no intersection (ray misses the sphere).
        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        // Calculate the square root of the discriminant, as we'll need it to find the intersection points.
        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root (t-value) that lies within the acceptable range.
        // This is the distance from the ray origin to the intersection point on the sphere.
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {  // Check if this root is within the interval
            // If the first root is not in the range, try the second possible root.
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root)) // If both roots are out of range, there's no hit
                return false;
        }

        // If an intersection is found within the acceptable range, we record the hit details:
        rec.t = root;                 // The t-value of the intersection point
        rec.p = r.at(rec.t);          // Calculate the exact hit point on the sphere surface

        // Calculate the outward normal vector at the intersection point.
        // The normal is derived by subtracting the sphere's center from the hit point
        // and dividing by the radius to ensure it’s unit length.
        vec3 outward_normal = (rec.p - center) / radius;

        // Set the hit record's normal, ensuring it faces against the ray's direction if needed.
        rec.set_face_normal(r, outward_normal);

        // Store the sphere's material in the hit record for shading or further processing.
        rec.mat = mat;

        return true;  // The ray hit the sphere within the acceptable range
    }

  private:
    point3 center = {};               // Sphere center point
    double radius = {};               // Sphere radius
    shared_ptr<material> mat = {};    // Material of the sphere
};

#endif
