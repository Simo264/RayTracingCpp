#include "Renderer.hpp"
#include "Scene.hpp"
#include "Ray.hpp"
#include "Material/IMaterial.hpp"
#include "Material/Emissive.hpp"

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
 *  - k_e if (n dot o) > 0
 *  - 0 otherwise
 * 
 * We model the reflected illumination for matte surfaces by randomly choosing a direction, 
 * with probability proportional to the cosine with the normal, 
 * and computing the reflected radiance L_r(p,o) as the product of the incoming illumination L_i(p, i)
 * at the point and the material color k_c.
 * In the product, we skip the cosine since we account for it by choosing incoming directions appropriately.
 * 
 * L_r(p,o) = k_c*L_i(p,i)
 * The incoming illumination L_i(p,i) is either the illumination L_o(q,-i) leaving the first visible point, q, 
 * along the ray (p,i) or the environment illumination E(i) if no intersection occurs.
 * L_i(p,i) =
 *  - L_o(q,-i) if q = raytrace(p,i)
 *  - E(i) if no intersection
 * 
 * You may be wandering how we account for illumination from light sources since we do not specifically handle them. 
 * In our shading model, we gather illumination from light source indirectly since emission is accounted with the emitted 
 * illumination. So if any ray in the incoming directions hits an emitting surface, it will pick up the emission from it. 
 * In fact, shadows from emissive surfaces are also accounted for. 
 * In this case, what will happen is that some rays sent toward the light will hit the emissive surface while nearby 
 * rays may hit an occluder, forming a visible shadow.
 */

glm::vec3 Renderer::computeRayColor(const Ray& ray, 
																		const Scene& scene, 
																		uint32_t depth) const
{
	if (depth <= 0)
		return glm::vec3(0.f);

	constexpr auto t_min = 1e-3;
	constexpr auto t_max = std::numeric_limits<float>::infinity();
	auto hit_record = HitRecord{};
	if (!scene.rayCasting(ray, t_min, t_max, hit_record))
	{
		return glm::vec3(0.f);

		//auto unit_direction = glm::normalize(ray.direction);
		//auto a = (unit_direction.y + 1.0f) * 0.5f;
		//return glm::mix(glm::vec3(1.f), glm::vec3(0.5f, 0.7f, 1.0f), a); // linear interpolation between blue and white
	}

	// 1. Luce emessa dalla superficie stessa (se è una sorgente luminosa)
	auto emitted_color = hit_record.material->emitted(hit_record.tc_u, hit_record.tc_v);
	auto scattered_ray = Ray();
	auto material_scatter_color = glm::vec3();
	// Se il materiale non disperde luce (es. è una luce pura), restituiamo solo il colore emesso.
	if (!hit_record.material->scatter(ray, hit_record, material_scatter_color, scattered_ray))
		return emitted_color;

	// 2. Illuminazione Diretta: campionamento esplicito delle luci
	auto direct_illumination = glm::vec3(0.0f);
	auto lights = scene.getEmissiveObjects();
	for (const auto& light : lights)
	{
		auto to_light_direction = glm::normalize(light->getPosition() - hit_record.point);
		auto shadow_ray = Ray(hit_record.point, to_light_direction);
		auto shadow_hit_record = HitRecord{};
		if (!scene.rayCasting(shadow_ray, t_min, glm::distance(light->getPosition(), hit_record.point), shadow_hit_record))
		{
			auto emissive_light = std::dynamic_pointer_cast<Emissive>(light);
			auto light_source_color = emissive_light->emission_scale;
			auto distance_squared = glm::length(light->getPosition() - hit_record.point);
			distance_squared *= distance_squared;

			auto attenuation = light_source_color / distance_squared;
			auto cosine_term = glm::max(glm::dot(hit_record.normal, to_light_direction), 0.0f);

			// Qui si moltiplica la luce diretta per il fattore di riflettanza del materiale.
			direct_illumination += material_scatter_color * attenuation * cosine_term;
		}
	}

	// 3. Illuminazione Indiretta: il rimbalzo ricorsivo
	// Si usa la ricorsione per calcolare l'illuminazione che proviene da altre superfici.
	auto indirect_illumination = material_scatter_color * computeRayColor(scattered_ray, scene, depth - 1);

	// 4. Risultato finale: somma di tutti i contributi
	// Si sommano i contributi diretti e indiretti, e si aggiunge la luce emessa.
	return emitted_color + direct_illumination + indirect_illumination;
}


/**
 * ============================================
 *		PRIVATE
 * ============================================
 */

