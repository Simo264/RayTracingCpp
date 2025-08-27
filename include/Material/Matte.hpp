#pragma once

#include "IMaterial.hpp"

class Matte : public IMaterial
{
public:
	Matte(glm::vec3 color_scale) : IMaterial()
	{
		this->color_scale = color_scale;
	}
	Matte(std::shared_ptr<Texture2D> color_texture) : IMaterial()
	{
		this->color_scale = glm::vec3(1.f);
		this->color_texture = color_texture;
	}
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
};
