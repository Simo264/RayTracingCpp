#pragma once

#include "IMaterial.hpp"

class Matte : public IMaterial
{
public:
	Matte(glm::vec3 color_scale)
		: IMaterial(color_scale, nullptr)
	{}
	// For textured matte
	Matte(std::shared_ptr<Texture2D> color_texture)
		: IMaterial(glm::vec3(1.f), color_texture)
	{}
	~Matte() = default;

	/**
	 * @brief 
	 * For a matte material, this bounce should be diffuse, meaning the light scatters uniformly 
	 * in many directions rather than reflecting sharply.
	 */
	bool scatter(const Ray& incident,
							 const HitRecord& hit,
							 glm::vec3& surface_color,
							 Ray& scattered_ray) const;
	
	glm::vec3 emitted(const HitRecord& hit) const { return glm::vec3(0.f); }
};
