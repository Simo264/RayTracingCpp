#include "Material/Lambertian.hpp"

#include "Ray.hpp"
#include "Random.hpp"
#include "Scene/IHittableObject.hpp"

bool Lambertian::scatter(const Ray& incident,
												 const HitRecord& rec,
												 Ray& scattered_ray,
												 glm::vec3& attenuation)
{
	attenuation = __albedo;

	// Implementing Lambertian distribution:
	// We generate a new direction by adding the surface normal to a random
	// unit vector. This simple operation naturally biases the resulting
	// direction towards the normal, creating a more realistic, non-uniform
	// distribution where rays are more likely to scatter near the normal.
	auto scattered_direction = rec.normal + Random::generateRandomUnitVector3();

	// If the random unit vector we generate is exactly opposite the normal vector, 
	// the two will sum to zero, which will result in a zero scatter direction vector.
	// This leads to bad scenarios later on (infinities and NaNs), so we need to intercept the condition before we pass it on.
	auto near_zero = [](glm::vec3 v) -> bool {
		constexpr float s = 1e-6f;
		return glm::abs(v.x) < s && glm::abs(v.y) < s && glm::abs(v.z) < s;
	};
	if (near_zero(scattered_direction))
		scattered_direction = rec.normal;
	
	scattered_ray = Ray(rec.p, scattered_direction);
	return true;
}
