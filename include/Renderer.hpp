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
};
