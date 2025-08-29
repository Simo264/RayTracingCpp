#include "Renderer.hpp"
#include "Scene.hpp"
#include "Ray.hpp"
#include "Material/IMaterial.hpp"

#include "Geometry/Sphere.hpp"
#include "Geometry/Plane.hpp"

#include <limits>
#include <glm/gtx/norm.hpp>		// glm::length2

/**
 * ============================================
 *		PUBLIC
 * ============================================
 */

/** 
 * 5.13.Shading
 * We can generate more realistic images by simulating the interaction of light with surfaces.
 * This is sometimes called shading.
 * To compute the illumination reflected by the surface, we need to determine how much illumination reaches
 * a small patch on the surface from a thin beam of light emitted by the light source towards the surface.
 * The amount of illumination that reaches a point from a light beam depends on the cosine between the
 * beam direction, i, and the surface normal, n.
 * 
 * The second aspect of this computation is to determine how much illumination the beam carries when it reaches the surface.
 * The farther away a light source is, the less it illuminates objects.
 * For point lights, the amount of illumination that reaches a surface from a point light decreases with the square root
 * of the distance between the light and the surface.
 * 
 * If we indicate with I the intensity of a light source at position, s, and with L_i(p, i) the illumination
 * that comes at a point from the direction, i, we can write the illumination due to a point light source as
 * L_i(p, i) = I / ||s - p || ^ 2
 * 
 * We can compute the illumination L_r(p, o) reflected by a surface point, p, in the direction, o, as the product
 * of the surface color, k_c, the light intensity I, the cosine of the angle between the incoming direction, i,
 * and the surface normal(n), divided by the square of the distance between the light position(s) and the surface point(p).
 * The direction, o, is just the opposite of the ray direction, d, so o = -d.
 * We can write this shading model as :
 * L_r(p, o) = kc * (I / ||s - p || ^ 2) * max(n dot i, 0)
 * 
 * Shadows can be added quite easily for this case.
 * A point is in shadows if there is an object blocking the light in its path from the light source to the surface.
 * We can test for occlusion by casting a ray from the shaded point to the light source, and check whether there is
 * an intersection and whether the intersection's distance is less than the distance to the light.
 * It is common to encode this test as a visibility function V(p, s) that is one if the point is visible from
 * the light source, and zero otherwise.
 * We can write shading with shadows as :
 * L_r(p, o) = kc * (I / ||s - p || ^ 2) * V(p, s) * max(n dot i, 0)
 * 
 * We can easily support multiple lights by summing over a list of point lights, and add a constant term, I_a,
 * to account for the average illumination of the environment coming from all other source.
 *
 * 
 * 
 * 5.15.Computing Illumination
 * In the real-world, illumination comes to a point not just from point lights, but from all other surfaces 
 * either because they emit light or because they reflect it. 
 * In the previous shading model, we use an ambient lighting term to account for this. 
 * To compute realistic illumination, we want instead to gather light from all possible directions, 
 * not just from the lights. 
 * This is not strictly possible since we cannot sample all possible directions. 
 * Instead, we will compute an approximation by picking a fixed, but rather large, number of directions and gather 
 * illumination from those.
 * But picking these direction is problematic since we do not know which ones carry the most light. 
 * Instead we'll just pick them at random, with each pixel picking different directions.
 * From these random directions, we gather illumination by sending a ray from the shaded point towards the random direction. 
 * At the intersected points, we recursively evaluate the same procedure, up to a fixed number of bounces.
 * But written this way, we would get an exponential explosion in the number of rays. 
 * We use many camera rays to get anti-aliasing, and for each of them, we would need many rays to estimate illumination.
 * This gives an exponential growth of computation that is not feasible if implemented verbatim. 
 * To solve this issue we observe that if we are already taking multiple rays for each pixel, 
 * we can just use a single direction, chosen at random, to estimate the illumination for each camera ray.
 *
 * So far, we simply mention random directions without begin specific about how to generate them. 
 * One possibility would be to send rays uniformly in the hemisphere above the shaded point. 
 * To start writing this more realistic model, we first define the illumination L_o(p,o) that leaves a point, p, 
 * in the direction, o, as the sum of the emitted illumination L_e(p, o) and the reflected one L_r(p,o).
 * The emitted illumination models light source that emit light on their own. 
 * The reflected one model all other materials that reflect light from the environment around them.
 * The emitted illumination is a property of the surface material. 
 * We model surfaces that emit light by setting the emitted illumination L_e(p,o) as a material value k_e.
 * We write the emitted radiance as:
 * L_e(p,o) = 
 *	- k_e if (n dot o) > 0
 *	- 0 otherwise
 */

glm::vec3 Renderer::computeRayColor(const Ray& ray, 
																		const Scene& scene, 
																		uint32_t depth) const
{
	// check correctly stops the recursion, preventing infinite loops
	if (depth <= 0)
		return glm::vec3(0.f);

	constexpr auto t_min = 0.001f;
	constexpr auto t_max = std::numeric_limits<float>::infinity();
	auto hit_record = HitRecord{};
	if (!scene.rayCasting(ray, t_min, t_max, hit_record))
	{
		return glm::vec3(0.f);
		
		//auto unit_direction = glm::normalize(ray.direction);
		//auto a = (unit_direction.y + 1.0f) * 0.5f;
		//return glm::mix(glm::vec3(1.f), glm::vec3(0.5f, 0.7f, 1.0f), a); // linear interpolation between blue and white
	}
	
	// Get emitted light from the material itself (if it's a light source).
	auto emitted_color = hit_record.material->emitted(hit_record.tc_u, hit_record.tc_v);
	auto scattered_ray = Ray();
	auto surface_color = glm::vec3();
	// If the material does not scatter light (e.g., it's a pure light source), return only its emitted color.
	if (!hit_record.material->scatter(ray, hit_record, surface_color, scattered_ray))
		return emitted_color;
	
	// Indirect Illumination
	auto color_from_scatter = surface_color * computeRayColor(scattered_ray, scene, depth - 1);
	return emitted_color + color_from_scatter;


#if 0
	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth == 0)
		return glm::vec3(0.f);

	constexpr auto t_min = 0.001f;
	constexpr auto t_max = std::numeric_limits<float>::infinity();
	auto hit_record = HitRecord{};

	// If the ray hits nothing, return the background (ambient) color.
	if (!scene.rayCasting(ray, t_min, t_max, hit_record))
	{
		return glm::vec3(0.f);

		//auto unit_direction = glm::normalize(ray.direction);
		//auto a = (unit_direction.y + 1.0f) * 0.5f;
		//return glm::mix(glm::vec3(1.f), glm::vec3(0.5f, 0.7f, 1.0f), a); // linear interpolation between blue and white
	}

	// Get emitted light from the material itself (if it's a light source).
	auto emitted_color = hit_record.material->emitted(hit_record.tc_u, hit_record.tc_v);

	// Determine how the ray scatters.
	auto scattered_ray = Ray();
	auto attenuation = glm::vec3();
	auto is_scattered = hit_record.material->scatter(ray, hit_record, attenuation, scattered_ray);

	// If the material does not scatter light (e.g., it's a pure light source), return only its emitted color.
	if (!is_scattered)
		return emitted_color;

	// Calculate direct illumination from explicit light sources.
	//auto direct_illumination = __calculateDirectIllumination(t_min, scene, hit_record);
	auto direct_illumination = glm::vec3(0.f);
	auto light_sources = scene.getEmissiveObjects();
	for (const auto& light_source : light_sources)
		direct_illumination += __calculateDirectIllumination(t_min, scene, hit_record, light_source);

	// Calculate indirect illumination through recursive path tracing.
	auto indirect_illumination = computeRayColor(scattered_ray, scene, depth - 1);

	// Combine all contributions: k_emitted + k_color * (direct_light + indirect_light).
	return emitted_color + attenuation * (direct_illumination + indirect_illumination);
#endif
}


/**
 * ============================================
 *		PRIVATE
 * ============================================
 */

glm::vec3 Renderer::__calculateDirectIllumination(float t_min,
																									const Scene& scene,
																									const HitRecord& record,
																									const std::shared_ptr<IHittableObject>& light_source) const
{
	

	// Get the light's position and material from the IHittableObject.
	const auto& light_position = light_source->getPosition();
	const auto& light_material = light_source->getMaterial();
	auto emissive_color = light_material->emitted(0, 0);
	auto light_dir = glm::normalize(light_position - record.point);
	auto light_distance = glm::length(light_position - record.point);


	// Cast a shadow ray from the hit point towards the light source.
	auto shadow_ray = Ray(record.point, light_dir);
	auto shadow_hit = HitRecord{};
	// Offset the shadow ray origin slightly along the normal to avoid self-intersection.
	shadow_ray.origin = record.point + record.normal * 1e-4f;

	// Check if there is an object between the hit point and the light source.
	if (scene.rayCasting(shadow_ray, t_min, light_distance, shadow_hit))
		return glm::vec3(0.f);

	auto direct_illumination = glm::vec3(0.f);

	// Calculate the cosine of the angle between the normal and the light direction.
	// Only apply illumination if the surface is facing the light.
	auto cos_theta = glm::dot(record.normal, light_dir);
	if (cos_theta > 0.f)
	{
		// Calculate light intensity with falloff based on the squared distance.
		// The emissive factor controls the overall brightness.
		auto light_intensity = emissive_color / (light_distance * light_distance);
		direct_illumination = light_intensity * cos_theta;
	}

	return direct_illumination;
}

