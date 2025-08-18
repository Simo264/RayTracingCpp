#include "Material/Metal.hpp"

#include "Scene/IHittableObject.hpp"
#include "Ray.hpp"
#include "Random.hpp"

bool Metal::scatter(const Ray& incident,
										const HitRecord& rec,
										Ray& scattered_ray,
										glm::vec3& attenuation)
{
	attenuation = __albedo;

	// Calculate perfect reflection
	auto reflected = glm::reflect(incident.direction(), rec.normal);
	// Apply fuzz (if any)
	if (__fuzz != 0.f)
		reflected = glm::normalize(reflected + __fuzz * Random::generateUnitVector());
	
	Ray scattered(rec.p, reflected);
	if (glm::dot(scattered.direction(), rec.normal) > 0.f)
	{
		scattered_ray = scattered;
		return true;
	}

	// no scatter
	return false;
}
