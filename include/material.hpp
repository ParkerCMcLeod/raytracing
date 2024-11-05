#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.hpp"

// Abstract base class for materials, providing a common interface for different types of materials.
// A material is responsible for how a ray interacts with an object - e.g., reflection, refraction, absorption, etc.
class material {
  public:
    virtual ~material() = default;

    // Scatter method, defining how rays interact with the surface.
    // Parameters:
    // - r_in: incoming ray that hits the surface.
    // - rec: information about the hit point (position, normal, etc.).
    // - attenuation: represents how much the color intensity is reduced.
    // - scattered: output parameter for the scattered ray after interaction.
    // Returns: true if the ray is scattered, false otherwise.
    virtual bool scatter(
        [[maybe_unused]] const ray& r_in, [[maybe_unused]] const hit_record& rec, [[maybe_unused]] color& attenuation, [[maybe_unused]] ray& scattered
    ) const {
        return false;
    }
};

// Lambertian (diffuse) material: scatters light uniformly in all directions.
class lambertian : public material {
  public:
    // Constructor to initialize the albedo, which defines the material color.
    lambertian(const color& albedo) : albedo(albedo) {}

    // Scatter method for lambertian material.
    // It generates a scattered ray in a random direction biased by the normal, simulating a matte surface.
    bool scatter([[maybe_unused]] const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        // Scatter direction is a random direction that is biased by the normal at the hit point.
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction: when the scattered direction is zero, fallback to the normal.
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        // Create the scattered ray from the hit point in the direction of the scatter.
        scattered = ray(rec.p, scatter_direction);
        
        // Attenuation represents the color and intensity of the ray after the hit.
        attenuation = albedo;
        
        return true;
    }

  private:
    // Albedo of the material, representing how much light is absorbed or reflected (the surface color).
    color albedo = {};
};

// Metal material: reflects rays, simulating metallic surfaces.
class metal : public material {
  public:
    // Constructor with albedo and fuzziness for how rough the metal surface appears.
    // Fuzziness controls the random spread of reflection; it's clamped to 1 to prevent extreme scattering.
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    // Scatter method for metal material.
    // Reflects the incoming ray in the direction dictated by the normal, adjusted by fuzziness for rough surfaces.
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        // Reflect the incoming ray direction around the surface normal.
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        
        // Add a small random deviation proportional to fuzz to simulate surface roughness.
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        
        // Create the scattered ray from the hit point in the adjusted reflection direction.
        scattered = ray(rec.p, reflected);
        
        // Attenuation for metal is the albedo, which gives it its color/reflective properties.
        attenuation = albedo;
        
        // Only accept reflections that are pointing away from the surface.
        return (dot(scattered.direction(), rec.normal) > 0);
    }

  private:
    color albedo = {}; // Albedo for the metal, determining its color.
    double fuzz = {};  // Fuzziness factor, determining the roughness of the metal surface.
};

// Dielectric (transparent) material: simulates glass-like surfaces with refraction and reflection.
class dielectric : public material {
  public:
    // Constructor with refractive index, describing how much the material bends light.
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    // Scatter method for dielectric materials.
    // Determines if the ray should reflect or refract based on the refractive index.
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        // Glass absorbs no color, so attenuation is set to 1 (full transmission).
        attenuation = color(1.0, 1.0, 1.0);
        
        // Ratio of refractive indices depending on whether the ray is entering or exiting the material.
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        // Unit vector of the incoming ray direction.
        vec3 unit_direction = unit_vector(r_in.direction());
        
        // Calculate the cosine of the angle between the ray and the normal.
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        
        // Calculate the sine of the angle (using trigonometric identity sin^2 + cos^2 = 1).
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        // Check for total internal reflection (when the ray cannot pass through the surface).
        bool cannot_refract = ri * sin_theta > 1.0;

        // Decide whether to reflect or refract.
        vec3 direction = {};
        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal); // Reflect the ray.
        else
            direction = refract(unit_direction, rec.normal, ri); // Refract the ray.

        // Create the scattered ray from the hit point in the calculated direction.
        scattered = ray(rec.p, direction);
        
        return true;
    }

  private:
    double refraction_index = {}; // Refractive index, determining the bending of light through the material.

    // Schlick's approximation to estimate the reflectance for a given angle and refractive index.
    static double reflectance(double cosine, double refraction_index) {
        // Approximation for reflectance at a given angle.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow((1 - cosine),5);
    }
};

#endif
