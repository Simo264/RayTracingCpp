#pragma once

#include "IMaterial.hpp"

/**
 * @brief Represents a Lambertian (diffuse) material that scatters light according to a cosine-weighted distribution.
 *
 * This class models true Lambertian reflection, where scattered rays are more likely to be emitted
 * in directions close to the surface normal. It provides a more realistic approximation of diffuse
 * surfaces than uniform scattering. The material always scatters incoming rays and attenuates them
 * based on its albedo.
 */
class Lambertian : public IMaterial
{
public:
	Lambertian(glm::vec3 albedo = glm::vec3(0.f)) // default albedo
		: __albedo{ albedo } {}
	~Lambertian() = default;

	/**
	 * @brief Computes the scattered ray based on Lambertian reflection.
	 * This method generates a new ray originating from the hit point and pointing
	 * in a direction biased toward the surface normal. The attenuation is set to
	 * the albedo, which scales the ray's color. The method always returns a scattered ray.
	 */
	bool scatter(const Ray& incident,
							 const HitRecord& rec,
							 Ray& scattered_ray,
							 glm::vec3& attenuation);
	
private:
	// The fractional reflectance and color of the material.
	// Albedo defines how much light is reflected by the surface. 
	// It is constant for this material and does not vary with viewing angle or wavelength.
	glm::vec3 __albedo;
};
