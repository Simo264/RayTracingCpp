#include "Material/Metal.hpp"
#include "Geometry/IHittableObject.hpp"

#include "Ray.hpp"

#include <glm/gtx/norm.hpp>		// glm::length2
#include <glm/gtc/random.hpp> // glm::sphericalRand

bool Metal::scatter(const Ray& incident,
										const HitRecord& hit,
										glm::vec3& surface_color,
										Ray& scattered_ray) const
{
	// This gives the ideal mirror reflection direction.
	auto reflected = glm::reflect(glm::normalize(incident.direction), hit.normal);

	// Roughness controls how blurry the reflection is.
	// If a texture is provided, it modulates the scalar roughness per pixel.
	auto roughness = roughness_scale;
	if (roughness_texture != nullptr)
	{
		auto tex_color = roughness_texture->sample(hit.tc_u, hit.tc_v).r;
		roughness = roughness_scale * tex_color;
	}


	// La soluzione più semplice ed efficace è perturbare il vettore riflesso con un rumore casuale generato 
	// non su una sfera completa, ma su un emisfero che punta nella direzione del vettore normale. 
	// In questo modo, il raggio perturbato sarà sempre sopra la superficie. 
	// Questo approccio è più coerente con il concetto di microfacets.
	auto random_dir = glm::normalize(hit.normal + glm::sphericalRand(1.0f));
	auto dir = glm::normalize(glm::mix(random_dir, reflected, glm::clamp(1.0f - roughness, 0.0f, 1.0f)));
	scattered_ray = Ray(hit.point, dir);


	// Determine Surface Color
	surface_color = color_scale;
	if (color_texture != nullptr)
		surface_color = color_scale * color_texture->sample(hit.tc_u, hit.tc_v);

	// Ensures the scattered ray is above the surface
	return glm::dot(scattered_ray.direction, hit.normal) > 0;
}

