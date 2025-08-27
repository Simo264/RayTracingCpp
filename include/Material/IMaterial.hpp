#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Texture/Texture2D.hpp"

struct HitRecord;
class Ray;

/** 
 * 4.6. Surface Materials
 * Objects' appearance depend on the interaction between surface materials and scene illumination. 
 * When light interacts with a surface, it is either scattered in different directions or absorbed by the object. 
 * For opaque surfaces, light is scattered above the surface, and the directionality of the scattered light determines 
 * whether an object looks dull or shiny.
 * For transparent surfaces, light is mostly scattered toward the object interior, making the objects see-through.
 * In general, the proportion of absorbed and scattered light differs for each wavelength, making objects appear colored.
 * In the physical world, surfaces interact with lights in a variety of different manners. 
 * But to capture their appearance approximately, we need to model only a handful of surface-light interactions that reproduce 
 * the look of matte, glossy, reflective, transparent and translucent surfaces.
 * 
 * One possibility is to model materials using a tag to indicate the material type, the surface color and the surface roughness. 
 * The material type differentiate between different light scattering behaviors. 
 * The surface color is the main color of the object. 
 * The surface roughness controls the sharpness of reflections and transmissions, for example going from a mirror to a dull metal.
 * 
 * In addition to reflecting light, surfaces may also emit it. 
 * Examples of these surfaces are the screen of a phone, the filament of a light bulb, etc. 
 * The simplest manner to model this behavior is to include an emission term to the material definition.
 * 
 * 4.8. Materials Representation
 * we can see that materials can be represented by a collection of parameters, such as the 
 * material type, emission, color, roughness, etc., together with references to the corresponding textures.
 * The most common case in graphics is to have most parameters textured, while only a few, like emission, 
 * remain constant over the surface.
 * A sketch of a possible material representation could be written as:
 * M = [
 *	type							: { matte, glossy, metal, transparent, translucent },
 *	emission scale		: glm::vec3,
 *	color scale				: glm::vec3 between [0-1],
 *	roughness scale		: float between [0-1],
 *	emission texture	: texture,
 *	color texture			: texture,
 *	roughness texture	: texture
 * ]
 *
 * 5.13.Shading
 * We can generate more realistic images by simulating the interaction of light with surfaces.
 * This is sometimes called shading.
 * To compute the illumination reflected by the surface, we need to determine how much illumination reaches
 * a small patch on the surface from a thin beam of light emitted by the light source towards the surface.
 * The amount of illumination that reaches a point from a light beam depends on the cosine between the
 * beam direction(i) and the surface normal(n).
 *
 * The second aspect of this computation is to determine how much illumination the beam carries when it reaches the surface.
 * The farther away a light source is, the less it illuminates objects.
 * For point lights, the amount of illumination that reaches a surface from a point light decreases with the square root
 * of the distance between the light and the surface.
 *
 * If we indicate with(I) the intensity of a light source at position(s) and with Li(p, i) the illumination
 * that comes at a point from the direction(i), we can write the illumination due to a point light source as
 * Li(p, i) = I / ||s - p || ^ 2
 *
 * We can compute the illumination Lr(p, o) reflected by a surface point(p) in the direction(o) as the product
 * of the surface color(kc), the light intensity I, the cosine of the angle between the incoming direction(i)
 * and the surface normal(n), divided by the square of the distance between the light position(s) and the surface point(p).
 * The direction(o) is just the opposite of the ray direction(d), so o = -d.
 * We can write this shading model as :
 * Lr(p, o) = kc * (I / ||s - p || ^ 2) * max(n dot i, 0)
 *
 * Shadows can be added quite easily for this case.
 * A point is in shadows if there is an object blocking the light in its path from the light source to the surface.
 * We can test for occlusion by casting a ray from the shaded point to the light source, and check whether there is
 * an intersection and whether the intersection's distance is less than the distance to the light.
 * It is common to encode this test as a visibility function V(p, s) that is one if the point is visible from
 * the light source, and zero otherwise.
 * We can write shading with shadows as :
 * Lr(p, o) = kc * (I / ||s - p || ^ 2) * V(p, s) * max(n dot i, 0)
 *
 * We can easily support multiple lights by summing over a list of point lights, and add a constant term(Ia)
 * to account for the average illumination of the environment coming from all other source.
 */

class IMaterial
{
public:
	IMaterial() : 
		color_scale{ glm::vec3(0.f) },
		emission_scale{ glm::vec3(0.f) },
		roughness_scale{ 0.f },
		color_texture{ nullptr },
		roughness_texture{ nullptr },
		emission_texture{ nullptr }
	{}
	virtual ~IMaterial() = default;

	// Core attributes
	glm::vec3 color_scale;
	glm::vec3 emission_scale;
	float roughness_scale;

	std::shared_ptr<Texture2D> color_texture;
	std::shared_ptr<Texture2D> roughness_texture;
	std::shared_ptr<Texture2D> emission_texture;

	/** @brief Determines how an incoming ray interacts with the surface, how it bounces off. */
	virtual bool scatter(const Ray& incident,
											 const HitRecord& hit,
											 glm::vec3& surface_color,
											 Ray& scattered_ray) const = 0;
	
	virtual glm::vec3 emitted(float u, float v) const
	{
		return glm::vec3(0.f);
	}
};

template<typename MaterialType, typename... Args>
inline std::shared_ptr<IMaterial> createMaterial(Args&&... args)
{
	static_assert(std::is_base_of<IMaterial, MaterialType>::value, "MaterialType must inherit from IMaterial");
	return std::make_shared<MaterialType>(std::forward<Args>(args)...);
}