#pragma once

#include <glm/glm.hpp>

#include "Geometry/IHittableObject.hpp"

class Scene;
class Ray;

class Renderer
{
public:
	Renderer() = default;
	~Renderer() = default;

	glm::vec3 computeRayColor(const Ray& ray, 
														const Scene& scene, 
														uint32_t depth) const;

private:
	glm::vec3 __calculateDirectIllumination(float t_min,
																					const Scene& scene, 
																					const HitRecord& record,
																					const std::shared_ptr<IHittableObject>& light_source) const;
};
