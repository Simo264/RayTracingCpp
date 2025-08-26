#include "Material/Metal.hpp"
#include "Geometry/IHittableObject.hpp"

#include "Ray.hpp"

#include <glm/gtx/norm.hpp>		// glm::length2
#include <glm/gtc/random.hpp> // glm::sphericalRand

bool Metal::scatter(const Ray& incident,
										const HitRecord& hit,
										glm::vec3& surcace_color,
										Ray& scattered_ray) const
{
	// This gives the ideal mirror reflection direction.
	auto reflected = glm::reflect(glm::normalize(incident.direction), hit.normal);

	// Roughness controls how blurry the reflection is.
	// If a texture is provided, it modulates the scalar roughness per pixel.
	float roughness = roughness_scale;
	if (roughness_texture != nullptr)
		roughness = roughness_scale * roughness_texture->sample(hit.tc_u, hit.tc_v).r;

	roughness = glm::clamp(roughness, 0.0f, 1.0f);

	// Perturb the reflection direction:
	// adds a random vector scaled by roughness to the perfect reflection.
	// This simulates microfacet scattering — the rougher the surface, the more the reflection deviates.
	auto dir = reflected + roughness * glm::sphericalRand(1.0f);
	scattered_ray = Ray(hit.point, glm::normalize(dir));

	// Determine Surface Color
	surcace_color = color_scale;
	if (color_texture != nullptr)
		surcace_color = color_scale * color_texture->sample(hit.tc_u, hit.tc_v);

	// Ensures the scattered ray is above the surface and
	// prevents rays from bouncing into the geometry.
	return glm::dot(scattered_ray.direction, hit.normal) > 0;
}

