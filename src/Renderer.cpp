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

	// If the ray hits nothing, return the background color.
	if (!scene.rayCasting(ray, t_min, t_max, record))
	{
		auto unit_direction = glm::normalize(ray.direction);
		auto a = (unit_direction.y + 1.0f) * 0.5f;
		return glm::mix(glm::vec3(1.f), glm::vec3(0.5f, 0.7f, 1.0f), a); // linear interpolation between blue and white
	}

	auto scattered_ray = Ray();
	auto surface_color = glm::vec3();
	auto color_from_emission = record.material->emitted(record.tc_u, record.tc_v);
	auto is_scattered = record.material->scatter(ray, record, surface_color, scattered_ray);
	if (!is_scattered)
		return color_from_emission;

	auto color_from_scatter = surface_color * computeRayColor(scattered_ray, scene, depth - 1);
	return color_from_emission + color_from_scatter;
}


/**
 * ============================================
 *		PRIVATE
 * ============================================
 */


