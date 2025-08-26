#pragma once

#include "IMaterial.hpp"

class Metal : public IMaterial
{
public:
	Metal(glm::vec3 color_scale,
				float roughness_scale,
				std::shared_ptr<Texture2D> roughness_texture = nullptr)
		: IMaterial(color_scale, nullptr, roughness_scale, roughness_texture)
	{}

	Metal(std::shared_ptr<Texture2D> color_texture,
				float roughness_scale,
				std::shared_ptr<Texture2D> roughness_texture = nullptr)
		: IMaterial(glm::vec3(1.f), color_texture, roughness_scale, roughness_texture)
	{}
	~Metal() = default;

	/**
	 * @brief
	 * Simulates perfect mirror reflection with a twist: it introduces roughness to perturb the reflection direction, 
	 * mimicking surface imperfections.
	 */
	bool scatter(const Ray& incident,
							 const HitRecord& hit,
							 glm::vec3& surface_color,
							 Ray& scattered_ray) const;

	glm::vec3 emitted(const HitRecord& hit) const { return glm::vec3(0.f); }
};
