#pragma once

#include "IMaterial.hpp"

/**
 * @brief Represents a metallic material with reflective properties.
 * The Metal class models a surface that reflects light in a mirror-like fashion,
 * with optional fuzziness to simulate imperfect reflections. It inherits from IMaterial
 * and implements the scatter method to determine how rays interact with the surface.
 */
class Metal : public IMaterial
{
public:
	/**
	 * @brief Constructs a Metal material with given albedo and fuzziness.
	 * @param albedo The base color of the metal surface (reflectance).
	 * @param fuzz The fuzziness factor controlling reflection blur.
	 *             Should be in the range [0.0, 1.0]. Values > 1.0 may cause unrealistic scattering.
	 */
	Metal(glm::vec3 albedo = glm::vec3(1.f),	// default albedo
				float fuzz = 0.f										// default fuzz no perturbation
	)
		: __albedo{ albedo }, __fuzz{ fuzz } {}
	~Metal() = default;

	/**
	 * @brief Computes how an incoming ray scatters off the metal surface.
	 * This method reflects the incoming ray about the surface normal and applies
	 * a fuzz factor to simulate imperfect reflections. If the scattered ray points
	 * outward from the surface, it is considered valid.
	 */
	bool scatter(const Ray& incident,
							 const HitRecord& rec,
							 Ray& scattered_ray,
							 glm::vec3& attenuation);

private:
	glm::vec3 __albedo;	// The base reflectance color of the metal surface.
	float __fuzz;				// The fuzziness factor. Higher values produce blurrier reflections.
                      // Recommended range: [0.0, 1.0]. Values above 1.0 may cause unrealistic behavior
};
