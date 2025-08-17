#pragma once

#include "IMaterial.hpp"

/**
 * @brief Represents a metallic material that reflects rays according to the law of reflection.
 * This class implements the IMaterial interface and models perfect specular reflection.
 * When a ray hits a surface with a Metal material, it reflects off the surface using
 * the reflection vector computed from the incident ray and the surface normal.
 * The material has an albedo that determines the color attenuation of the reflected ray.
 */
class Metal : public IMaterial
{
public:
	Metal(glm::vec3 albedo = glm::vec3(1.f),	// default albedo
				float fuzz = 0.f										// default fuzz no perturbation
	)
		: __albedo{ albedo }, __fuzz{ fuzz } {}
	~Metal() = default;

  /**
	 * @brief Computes the scattered ray for metallic reflection.
	 * Reflects the incident ray off the surface using the surface normal.
	 * Sets the attenuation to the material's albedo.
	 */
	bool scatter(const Ray& incident,
							 const HitRecord& rec,
							 Ray& scattered_ray,
							 glm::vec3& attenuation);

private:
	// The color attenuation of the material.
	glm::vec3 __albedo;

	float __fuzz;
};
