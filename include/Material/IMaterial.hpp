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
	
	virtual glm::vec3 emitted(float u, float v) const { return glm::vec3(0.f); }
};

template<typename MaterialType, typename... Args>
inline std::shared_ptr<IMaterial> createMaterial(Args&&... args)
{
	static_assert(std::is_base_of<IMaterial, MaterialType>::value, "MaterialType must inherit from IMaterial");
	return std::make_shared<MaterialType>(std::forward<Args>(args)...);
}