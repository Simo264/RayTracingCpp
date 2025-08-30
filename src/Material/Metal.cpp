#include "Material/Metal.hpp"
#include "Geometry/IHittableObject.hpp"

#include "Ray.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>		// glm::length2
#include <glm/gtc/random.hpp> // glm::sphericalRand

/**
 * For polished metals the ray won't be randomly scattered.
 * Polished reflective surfaces scatter light mostly around a single direction, 
 * and the appear like mirrors colored by the surface color k_c.
 * We can simulate these surfaces by sending rays in the reflected direction, i,
 * which is the outgoing direction, o, rotated around normal by 180 degrees, which is computed as:
 * i = -o + (2 dot (n dot o) dot n).
 * 
 * Most reflective surfaces are made of metals whose colors changes with the incoming angle, 
 * varying slightly from a colored surface, when viewed from above, to a perfectly white surface, 
 * when viewed at grazing angles. 
 * This behavior is explained by the Fresnel equations, that relate the surface color to the angle 
 * of incoming illumination. 
 * These equations are particularly complex for metals, and beyond our needs. 
 * Instead, we will use an approximation due to Christophe Schlick, that approximates the Fresnel term:
 * F_s(k_c, n, o) as the linear blend between the surface color k_c and white,
 * where the blending weight is the cosine of the incoming direction raised to the fifth power.
 * 
 * In the real - world, most surfaces are not as polished as the ones we rendered so far,
 * but have a rougher appearance either because they are naturally rougher or due to aging and weathering.
 * A large class of rough material can be simulated with microfacet models, that model statistically the material
 * response of surfaces made of tiny facets that scatter lights.
 * We consider a surface made of minuscule facets of the same polished material.
 * We model the surface by describing the distribution of the microfacets normals.
 * Naturally, most facets will be oriented along the microscopic surface normal, but some will have different orientations.
 * 
 * We can render rough surfaces in a relatively straightforward manner. For each ray, we randomly
 * select a microfacet normal, m, and then use that normal when computing the reflected direction, i.
 * This will result in reflected directions that are contained in a cone around the mirror direction.
 * The size of the cone depends on the surface roughness.
 */

bool Metal::scatter(const Ray& incident,
										const HitRecord& hit,
										glm::vec3& surface_color,
										Ray& scattered_ray) const
{
	// Step 1: Calculate the perfect reflected direction.
	// The incident direction is inverted because the formula expects a vector pointing away from the surface.
	auto incident_dir_normalized = glm::normalize(incident.direction);
	auto reflected = glm::reflect(incident_dir_normalized, hit.normal);

	// Step 2: Implement the Fresnel approximation.
	// Calculate the cosine of the angle between the incoming ray and the surface normal.
	// Use the absolute value to handle cases where the ray hits from the back of a thin object.
	auto cos_theta = glm::dot(-incident_dir_normalized, hit.normal);

	// Calculate the blending weight using the Schlick approximation.
	float r0 = glm::pow(1.0f - cos_theta, 5.0f);

	// Linearly interpolate between the base color and a white color (1.0, 1.0, 1.0).
	// The Fresnel term determines how much of the white color is blended in.
	auto kc = color_scale;
	if (this->color_texture != nullptr)
		kc = color_scale * color_texture->sample(hit.tc_u, hit.tc_v);

	auto fresnel_color = glm::mix(kc, glm::vec3(1.0f), r0);

	// Step 3: Set the surface color and scattered ray.
	surface_color = fresnel_color;
	scattered_ray = Ray(hit.point, reflected);

	// Step 4: Add roughness.
	// If the material has roughness, add a small random offset to the reflected direction.
	if (roughness_scale > 0.0f)
	{
		auto random_dir = glm::normalize(glm::sphericalRand(roughness_scale));
		scattered_ray = Ray(hit.point, glm::normalize(reflected + random_dir));
	}

	// Ensure the scattered ray is on the correct side of the surface.
	return glm::dot(scattered_ray.direction, hit.normal) > 0;
}

