#include "Renderer.hpp"
#include "Scene.hpp"
#include "Ray.hpp"
#include "Geometry/IHittableObject.hpp"
#include "Material/IMaterial.hpp"

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

	constexpr auto t_min = 0.001f;
	constexpr auto t_max = std::numeric_limits<float>::infinity();

	auto record = HitRecord{};
	if (scene.rayCasting(ray, t_min, t_max, record))
	{
		if (record.material == nullptr)
			return glm::vec3(0.f);

		auto attenuation = glm::vec3();
		auto scattered_ray = Ray();
		auto is_scattered = record.material->scatter(ray, record, attenuation, scattered_ray);
		if (!is_scattered)
			return attenuation;
		
		return attenuation * computeRayColor(scattered_ray, scene, depth - 1);
	}

	auto unit_direction = glm::normalize(ray.direction);
	auto a = (unit_direction.y + 1.0f) * 0.5f;
	return glm::mix(glm::vec3(1.f), glm::vec3(0.5f, 0.7f, 1.0f), a); // linear interpolation between blue and white
}


/**
 * ============================================
 *		PRIVATE
 * ============================================
 */


