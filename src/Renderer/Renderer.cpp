#include "Renderer/Renderer.hpp"
#include "Scene/Scene.hpp"
#include "Scene/IHittableObject.hpp"
#include "Material/IMaterial.hpp"
#include "Ray.hpp"
#include "Interval.hpp"

#include <limits>

/**
 * ============================================
 *		PUBLIC
 * ============================================
 */

glm::vec3 Renderer::computeRayColor(const Ray& ray, const Scene& scene, uint32_t depth) const
{
	if (depth == 0) // If we've exceeded the ray bounce limit, no more light is gathered.
		return glm::vec3(0.f);

	constexpr auto infinity = std::numeric_limits<float>::infinity();

	HitRecord record{};
	if (scene.hitAnything(ray, Interval(0.001f, infinity), record))
	{
		if (record.material == nullptr)
			return glm::vec3(0.f);

		glm::vec3 attenuation;
		Ray scattered_ray;
		auto is_scattered = record.material->scatter(ray, record, scattered_ray, attenuation);
		if(!is_scattered)
			return glm::vec3(0.f);
		
		return attenuation * computeRayColor(scattered_ray, scene, depth - 1);
	}

	auto unit_direction = glm::normalize(ray.direction());
	auto a = (unit_direction.y + 1.0f) * 0.5f;
	return glm::mix(glm::vec3(1.f), glm::vec3(0.5f, 0.7f, 1.0f), a); // linear interpolation between blue and white
}


/**
 * ============================================
 *		PRIVATE
 * ============================================
 */


